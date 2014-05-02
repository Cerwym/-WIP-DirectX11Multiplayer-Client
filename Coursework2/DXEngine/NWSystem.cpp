#include "NWSystem.h"

void NetworkRead(void* ptr);

NWSystem::~NWSystem()
{
	// Shutdown
}

bool NWSystem::Init()
{
	if (!InitWinSock())
		return false;

	return true;
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
	unsigned long IOSetting, inetAddr, startTime, threadID;
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

void NWSystem::SendDisconnectMessage()
{
	MSG_DISCONNECT_DATA message;
	int bytes_to_send;

	message.type = MSG_DISCONNECT;
	message.idNumber = m_IDNumber;
	message.sessionId = m_sessionID;

	bytes_to_send = sendto(m_clientSocket, (char*)&message, sizeof(MSG_DISCONNECT_DATA), 0, (struct sockaddr*)&m_serverAddress, m_addressLength);
	if (bytes_to_send != sizeof(MSG_DISCONNECT_DATA)){}

	return;
}

void NWSystem::ReadNetworkMessage(char* recvBuffer)
{
	MSG_GENERIC_DATA* message;
	
	// Coerce the message into a generic format to read the type of message.
	message = (MSG_GENERIC_DATA*)recvBuffer;

	switch(message->type)
	{
	case MSG_PING:
		{
			//HandlePingMessage();
			break;
		}
	default:
		{
			break;
		}
	}

	return;
}

//void NWSystem::Proc

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
			ClassPtr->ReadNetworkMessage( recvBuffer );
		}
	}

	ClassPtr->SetThreadState(false);

	printf("Client has stopped listening for server messages\n");
}