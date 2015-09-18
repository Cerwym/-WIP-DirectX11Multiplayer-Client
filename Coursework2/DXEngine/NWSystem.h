#pragma once

const int MAX_MESSAGE_SIZE = 512;
const int MAX_QUEUE_SIZE = 200;
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <mmsystem.h>
#include <iostream>
#include <thread>

#include "Objects/D3DUI.h"
#include "NWMessages.h"

class GameState;

// Will probably want this to be abstract, as in, not static in the case of it being either a client OR server

class NWSystem
{
private:
	struct QueueT
	{
		bool active;
		struct sockaddr_in address;
		int size;
		char message[MAX_MESSAGE_SIZE];
	};
public:
	NWSystem() { m_IsOnline = false; }
	NWSystem(const NWSystem&){}
	~NWSystem();

	bool Init(void* gs, D3DUI* uiPtr);
	bool ConnectToServer(char* IPAddress, unsigned short portNum, int timeOut);
	bool Online() {return m_IsOnline;}
	void Update( );
	SOCKET GetClientSocket() {return m_clientSocket;}
	int GetLatency(){ return m_Latency;}
	void ReadNetworkMessage(char* recvBuffer, int bytesRead, struct sockaddr_in serverAddress);
	void SetThreadState(bool state){ m_ThreadActive = state;}
	bool ShutdownNetwork();
	void SendDisconnectMessage();
	void SetVerbosity(bool flag){m_VerboseOutput = flag;}
	void SetUIAndGS(void* gs, D3DUI* ui);
	bool RequestEntityList();

	void HasRecievedMessage(bool flag){m_MessageRecieved = flag;}
	bool RecievedMessage(){return m_MessageRecieved;}

	// Set the amount of time in SECONDS to wait before sending another ping message.
	void SetPingWaitTime(float time){m_TickRate = time;}

	bool SendStateChange(char state);
	bool SendPositionUpdate(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 velocity, XMFLOAT3 acceleration);
	
private:
	void AddMessageToQueue(char* message, int messageSize, struct sockaddr_in serverAddress);
	void ProcessMessageQueue();
	bool InitWinSock();
	
	void HandleNewUserLoginMessage(int);
	void HandleUserDisconnectMessage(int);
	void HandleChatMessage(int);
	void HandlePingMessage();
	void HandleEntityInfoMessage(int);
	void HandleStateChangeMessage(int);
	void HandlePositionMessage(int);
	
	void ProcessLatency();	
	bool SendChatMessage(char* message);
	void SendPing();

	bool m_VerboseOutput;
	bool m_MessageRecieved;
	float m_TickRate;
	SOCKET m_clientSocket;
	D3DUI* m_uiPtr;
	GameState* m_GameState;
	std::thread m_listenThread;
	int m_addressLength;
	struct sockaddr_in m_serverAddress;
	unsigned short m_IDNumber, m_sessionID;
	bool m_IsOnline, m_ThreadActive;
	int m_Latency;
	unsigned long m_PingTime;
	QueueT* m_networkMessageQueue;
	int m_nextQueueLocation, m_nextMessageForProcessing;
	char m_chatMessage[64];
	char m_uiMessage[50];
};
