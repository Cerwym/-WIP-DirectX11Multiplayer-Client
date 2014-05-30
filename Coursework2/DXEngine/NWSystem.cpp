#include "NWSystem.h"
#include "..\GameState.h" // REMOVE THIS INCLUDE - I do not like the circular dependency.

void NetworkRead(void* ptr);

NWSystem::~NWSystem()
{
	// Shutdown
}

bool NWSystem::Init( void* gs = NULL, D3DUI* uiPtr = NULL)
{
	m_GameState = 0;
	m_uiPtr = 0;
	m_TickRate = 5000; // If not called, TickRate will default to 5 seconds
	m_VerboseOutput = false;

	// Initialize the 'queue' system.
	m_networkMessageQueue = new QueueT[MAX_MESSAGE_SIZE];

	m_nextQueueLocation = 0;
	m_nextMessageForProcessing = 0;

	for (int i = 0; i < MAX_QUEUE_SIZE; i++)
		m_networkMessageQueue[i].active = false;

	if (!InitWinSock())
		return false;

	if (uiPtr != NULL)
		m_uiPtr	= uiPtr;

	if (gs != NULL)
		m_GameState = (GameState*)gs;

	return true;
}

void NWSystem::SetUIAndGS(void* gs, D3DUI* ui)
{
	m_GameState = (GameState*)gs;
	m_uiPtr = ui;
}

bool NWSystem::InitWinSock()
{
	WSADATA SockData;
	WSAPROTOCOL_INFOW* protocolBuffer;
	unsigned long bufferSize;
	int protocols[2];
	
	int error = WSAStartup(0x0202, &SockData);
	if (error != 0)
	{
		// Handle WSAStartup Error
	}

	// Request the buffer size needed for holding the available protocols
	WSAEnumProtocols(NULL, NULL, &bufferSize);

	// Create a buffer for the protocol information structs
	protocolBuffer = new WSAPROTOCOL_INFOW[bufferSize];
	if (!protocolBuffer)
		return false;

	protocols[0] = IPPROTO_TCP;
	protocols[1] = IPPROTO_UDP;

	error = WSAEnumProtocols(protocols, protocolBuffer, &bufferSize);
	if (error == SOCKET_ERROR)
		return false;

	delete[] protocolBuffer;
	protocolBuffer = 0;

	return true;
}

bool NWSystem::ConnectToServer(char* IPAddress, unsigned short portNum, int timeOut)
{
	unsigned long IOSetting, inetAddr, startTime;
	MSG_GENERIC_DATA connectMessage;
	MSG_NEWID_DATA* message;
	int bytesSent, bytesRead;
	char recvBuffer[4096];
	IOSetting = 1;

	// Will want to expand this to also create a TCP socket for reliability needs

	// Create a UDP Socket.
	m_clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_clientSocket == INVALID_SOCKET)
		return false;

	// Set the socket to non-blocking (nonzero 3rd parameter)
	if ( ioctlsocket( m_clientSocket, FIONBIO, &IOSetting))
		return false;

	// Save the size of the server address structure.
	m_addressLength = sizeof(m_serverAddress);

	inetAddr = inet_addr(IPAddress);
	memset((char*)&m_serverAddress, 0, m_addressLength);
	m_serverAddress.sin_family = AF_INET;
	m_serverAddress.sin_addr.s_addr = inetAddr;
	m_serverAddress.sin_port = htons(portNum);

	// Setup a message to be sent that will indicate a new connection
	connectMessage.type = MSG_CONNECT;

	bytesSent = sendto(m_clientSocket, (char*)&connectMessage, sizeof(MSG_GENERIC_DATA), 0, (struct sockaddr*)&m_serverAddress, m_addressLength);
	if (bytesSent < 0)
		return false;

	// Record the time when the connect packet was sent.
	startTime = timeGetTime();

	bool done = false;
	bool obtainedID = false;

	while (!done)
	{
		// Check for a reply message from the server.
		bytesRead = recvfrom(m_clientSocket, recvBuffer, 4096, 0, (struct sockaddr*)&m_serverAddress, &m_addressLength);
		if ( bytesRead > 0)
		{
			done = true;
			obtainedID = true;
		}

		if (timeGetTime() > (startTime + timeOut))
		{
			done = true;
			obtainedID = false;
		}
	}

	// If an id (message) was not received in the established timeout period, the server was unreachable
	if (!obtainedID)
		return false;

	message = (MSG_NEWID_DATA*)recvBuffer;

	// Ensure it was an ID message
	if( message->type != MSG_NEWID)
		return false;

	// Store the ID number for this client for all further communications with the server
	m_IDNumber = message->idNumber;
	m_sessionID = message->sessionId;

	m_IsOnline = true;
	m_ThreadActive = false;

	// Create a thread to listen for network I/O from the server
	m_listenThread = std::thread( &NetworkRead,(void*)this);

	m_Latency = 0;
	m_PingTime = timeGetTime();

	if (m_GameState)
	{
		m_GameState->HandleNewPositionData(message->posX, message->posY, message->posZ);
	}
	printf("Client is now connected to the server!\n");
	return true;
}

bool NWSystem::ShutdownNetwork()
{
	// If the client was never online, return as there is no point in further processing
	if (!m_IsOnline)
		return true;

	// Send a disconnect message to inform the server of the client's intended state
	SendDisconnectMessage();
	
	m_IsOnline = false;

	// Wait for the network I/O thread to complete.
	while(m_ThreadActive){}

	closesocket(m_clientSocket);

	// Join the listen thread with the main thread since we no longer need it to be active.
	m_listenThread.join();

	// Shutdown winsock
	WSACleanup();

	return true;
}

void NWSystem::ReadNetworkMessage(char* recvBuffer, int bytesRead, struct sockaddr_in serverAddress)
{
	MSG_GENERIC_DATA* message;
	
	// check for buffer overflow
	if (bytesRead > MAX_MESSAGE_SIZE)
		return;

	// Coerce the message into a generic format to read the type of message.
	message = (MSG_GENERIC_DATA*)recvBuffer;
	if (message->type == MSG_PING)
	{
		HandlePingMessage();
	}
	else
	{
		AddMessageToQueue(recvBuffer, bytesRead, serverAddress);
	}

	return;
}

void NWSystem::Update()
{
	bool newMessage;

	ProcessLatency();

	ProcessMessageQueue();

	// Check to see if there is a chat message that the user wants to send to the server
	if (m_uiPtr != NULL)
	{
		m_uiPtr->CheckForChatMessage(m_uiMessage, newMessage);
		if (newMessage)
			SendChatMessage(m_uiMessage);
	}
}

void NWSystem::AddMessageToQueue(char* message, int messageSize, struct sockaddr_in serverAddress)
{
	// Check for buffer overflow.
	if (messageSize > MAX_MESSAGE_SIZE)
	{
		int i = 5;
		// BUFFER OVERFLOW
	}

	// Otherwise add it to the circular message queue to be processed.
	else
	{
		m_networkMessageQueue[m_nextQueueLocation].address = serverAddress;
		m_networkMessageQueue[m_nextQueueLocation].size = messageSize;
		memcpy(m_networkMessageQueue[m_nextQueueLocation].message, message, messageSize);

		// Set active last so that racing conditions in processing the queue do not exist.
		m_networkMessageQueue[m_nextQueueLocation].active = true;

		m_nextQueueLocation++;
		if (m_nextQueueLocation == MAX_QUEUE_SIZE)
			m_nextQueueLocation = 0;
	}
}

void NWSystem::ProcessMessageQueue()
{
	MSG_GENERIC_DATA* message;

	// loop through all the active unprocessed messages in the queue.
	while(m_networkMessageQueue[m_nextMessageForProcessing].active == true)
	{
		// coerce
		message = (MSG_GENERIC_DATA*)m_networkMessageQueue[m_nextMessageForProcessing].message;

		switch (message->type)
		{
		case MSG_CHAT:
			{
				HandleChatMessage(m_nextMessageForProcessing);
				break;
			}
		case MSG_ENTITY_INFO:
			{
				HandleEntityInfoMessage(m_nextMessageForProcessing);
				break;
			}
		case MSG_NEW_USER_LOGIN:
			{
				HandleNewUserLoginMessage(m_nextMessageForProcessing);
				break;
			}
		case MSG_USER_DISCONNECT:
			{
				HandleUserDisconnectMessage(m_nextMessageForProcessing);
				break;
			}
		case MSG_POSITION:
			{
				HandlePositionMessage(m_nextMessageForProcessing);
				break;
			}
		default:
			break;
		}
		// set the message as processed.
		m_networkMessageQueue[m_nextMessageForProcessing].active = false;

		m_nextMessageForProcessing++;
		if (m_nextMessageForProcessing == MAX_QUEUE_SIZE)
			m_nextMessageForProcessing = 0;
	}
}

void NWSystem::ProcessLatency()
{
	if (timeGetTime() >= (m_PingTime + m_TickRate))
	{
		m_PingTime = timeGetTime();
		SendPing();
	}
}

void NWSystem::HandlePingMessage()
{
	m_Latency = timeGetTime() - m_PingTime;
}

void NWSystem::HandleChatMessage(int queuePosition)
{
	MSG_CHAT_DATA* msg;
	unsigned short clientID;

	msg = (MSG_CHAT_DATA*)m_networkMessageQueue[queuePosition].message;
	clientID = msg->idNumber;

	strcpy_s(m_chatMessage, 64, msg->text);

	// If there was a new message then send it to he user interface.
	if (m_uiPtr != NULL)
		m_uiPtr->AddChatMessageFromServer(m_chatMessage, clientID);
}

void NWSystem::HandleEntityInfoMessage(int queuePosition)
{
	MSG_ENTITY_INFO_DATA* message;
	unsigned short entityID;
	char entityType;
	float posX, posY, posZ;
	float rotX, rotY, rotZ;

	// Get contents of the message.
	message = (MSG_ENTITY_INFO_DATA*)m_networkMessageQueue[queuePosition].message;
	entityID   = message->entityId;
	entityType = message->entityType;
	posX  = message->positionX;
	posY  = message->positionY;
	posZ  = message->positionZ;
	rotX  = message->rotationX;
	rotY  = message->rotationY;
	rotZ  = message->rotationZ;

	// If the game state has been set, add an entity to it.
	if (m_GameState)
	{
		m_GameState->AddEntity(entityID, entityType, posX, posY, posZ, rotX, rotY, rotZ);
	}
}

void NWSystem::HandleNewUserLoginMessage(int queuePosition)
{
	MSG_ENTITY_INFO_DATA* message;
	unsigned short entityID;
	char entityType;
	float posX, posY, posZ;
	float rotX, rotY, rotZ;

	message = (MSG_ENTITY_INFO_DATA*)m_networkMessageQueue[queuePosition].message;

	entityID = message->entityId;
	entityType = message->entityType;
	posX  = message->positionX;
	posY  = message->positionY;
	posZ  = message->positionZ;
	rotX  = message->rotationX;
	rotY  = message->rotationY;
	rotZ  = message->rotationZ;

	if (m_GameState)
		m_GameState->AddEntity( entityID, entityType, posX, posY, posZ, rotX, rotY, rotZ);
}

void NWSystem::HandleUserDisconnectMessage( int queuePosition )
{
	MSG_USER_DISCONNECT_DATA* message;
	unsigned short entityID;

	message = (MSG_USER_DISCONNECT_DATA*)m_networkMessageQueue[queuePosition].message;

	entityID = message->idNumber;
	if (m_GameState)
		m_GameState->RemoveEntity( entityID );
}

void NWSystem::HandleStateChangeMessage( int queuePosition )
{
	MSG_STATE_CHANGE_DATA* message;
	unsigned short entityID;
	char state;

	message = (MSG_STATE_CHANGE_DATA*)m_networkMessageQueue[queuePosition].message;

	entityID = message->idNumber;
	state = message->state;

	if (m_GameState)
		m_GameState->UpdateEntityState( entityID, state );
}

void NWSystem::HandlePositionMessage( int queuePosition )
{
	MSG_POSITION_DATA* message;
	unsigned short entityID;
	float posX, posY, posZ, rotX, rotY, rotZ;

	message = (MSG_POSITION_DATA*)m_networkMessageQueue[queuePosition].message;

	entityID = message->idNumber;
	posX = message->positionX;
	posY = message->positionY;
	posZ = message->positionZ;
	rotX = message->rotationX;
	rotY = message->rotationY;
	rotZ = message->rotationZ;

	if (m_GameState)
	{
		m_GameState->UpdateEntityPosition( entityID, posX, posY, posZ, rotX, rotY, rotZ);
	}
}

bool NWSystem::RequestEntityList()
{
	MSG_SIMPLE_DATA message;

	message.type = MSG_ENTITY_REQUEST;
	message.idNumber = m_IDNumber;
	message.sessionId = m_sessionID;

	int bytesSent = sendto(m_clientSocket, (char*)&message, sizeof(MSG_SIMPLE_DATA), 0, (struct sockaddr*)&m_serverAddress, m_addressLength);
		if (bytesSent != sizeof(MSG_SIMPLE_DATA))
			return false;

	return true;
}

bool NWSystem::SendChatMessage(char* inputMSG)
{
	MSG_CHAT_DATA message;

	// Create the message itself.
	message.type = MSG_CHAT;
	message.idNumber = m_IDNumber;
	message.sessionId = m_sessionID;
	strcpy_s(message.text, 64, inputMSG);

	// Send the message to the server.
	int bytesSent = sendto(m_clientSocket, (char*)&message, sizeof(MSG_CHAT_DATA), 0, (struct sockaddr*)&m_serverAddress, m_addressLength);
	if (bytesSent != sizeof(MSG_CHAT_DATA))
		return false;
	else
		if (m_VerboseOutput)
			g_Engine->DebugOutput(L"Sent Ping Message to Server");

	return true;
}

void NWSystem::SendPing()
{
	MSG_PING_DATA message;

	message.type = MSG_PING;
	message.idNumber = m_IDNumber;
	message.sessionId = m_sessionID;

	int bytesSent = sendto(m_clientSocket, (char*)&message, sizeof(MSG_PING_DATA), 0, (struct sockaddr*)&m_serverAddress, m_addressLength);
	if (bytesSent != sizeof(MSG_PING_DATA)){}
	else
		if (m_VerboseOutput)
			g_Engine->DebugOutput(L"Sent Ping Message to Server");
}

void NWSystem::SendDisconnectMessage()
{
	MSG_DISCONNECT_DATA message;
	int bytes_to_send;

	message.type = MSG_DISCONNECT;
	message.idNumber = m_IDNumber;
	message.sessionId = m_sessionID;

	bytes_to_send = sendto(m_clientSocket, (char*)&message, sizeof(MSG_DISCONNECT_DATA), 0, (struct sockaddr*)&m_serverAddress, m_addressLength);
	if (bytes_to_send != sizeof(MSG_DISCONNECT_DATA)){}
	else
		if (m_VerboseOutput)
			g_Engine->DebugOutput(L"Sent Ping Message to Server");

	return;
}

bool NWSystem::SendPositionUpdate(float x, float y, float z, float Rx, float Ry, float Rz)
{
	MSG_POSITION_DATA message;

	// create the message itself.
	message.type = MSG_POSITION;
	message.idNumber = m_IDNumber;
	message.sessionId = m_sessionID;
	message.positionX = x;
	message.positionY = y;
	message.positionZ = z;
	message.rotationX = Rx;
	message.rotationY = Ry;
	message.rotationZ = Rz;

	int bytesSent = sendto(m_clientSocket, (char*)&message, sizeof(MSG_POSITION_DATA), 0, (struct sockaddr*)&m_serverAddress, m_addressLength);
	if (bytesSent != sizeof(MSG_POSITION_DATA))
		return false;
	else
		if (m_VerboseOutput)
			g_Engine->DebugOutput(L"Sent Ping Message to Server");

	return true;
}

bool NWSystem::SendStateChange(char state)
{
	MSG_STATE_CHANGE_DATA message;

	message.type = MSG_STATE_CHANGE;
	message.idNumber = m_IDNumber;
	message.sessionId = m_sessionID;
	message.state = state;

	int bytesSent = sendto(m_clientSocket, (char*)&message, sizeof(MSG_STATE_CHANGE_DATA), 0, (struct sockaddr*)&m_serverAddress, m_addressLength);
	if (bytesSent != sizeof(MSG_STATE_CHANGE_DATA))
		return false;
	else
		if (m_VerboseOutput)
			g_Engine->DebugOutput(L"Sent Ping Message to Server");

	return true;
}

void NetworkRead(void* ptr)
{
	NWSystem* ClassPtr;
	struct sockaddr_in serverAddress;
	int addressLength;
	int bytesRead;
	char recvBuffer[4096];

	std::cout << "Client is now listening for incoming messages." << std::endl;

	ClassPtr = (NWSystem*)ptr;
	ClassPtr->SetThreadState(true);
	addressLength = sizeof(serverAddress);

	// Loop and read network messages while the client is online
	while(ClassPtr->Online())
	{
		// Check to see if there is a message from the server.
		bytesRead = recvfrom( ClassPtr->GetClientSocket(), recvBuffer, 4096, 0, (struct sockaddr*)&serverAddress, &addressLength);
		if (bytesRead > 0)
		{
			std::cout << "Reading network message." << std::endl;
			ClassPtr->ReadNetworkMessage( recvBuffer, bytesRead, serverAddress );
		}
	}

	ClassPtr->SetThreadState(false);

	printf("Client has stopped listening for server messages\n");
}