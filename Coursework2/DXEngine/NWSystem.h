#ifndef NWSYSTEM_H
#define NWSYSTEM_H

const int MAX_MESSAGE_SIZE = 512;
const int MAX_QUEUE_SIZE = 200;

// Message Types
#include "NWMessages.h"

// Temporary Struct, will change this later for more application related data
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <mmsystem.h>
#include <iostream>
#include <thread>

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

	bool Init();
	bool Online() {return m_IsOnline;}
	void ReadNetworkMessage(char* recvBuffer);
	SOCKET GetClientSocket() {return m_clientSocket;}
	bool ConnectToServer(char* IPAddress, unsigned short portNum, int timeOut);
	void SetThreadState(bool state){ m_ThreadActive = state;}
	bool ShutdownNetwork();
	
	void SendDisconnectMessage();
	

private:
	bool InitWinSock();

	SOCKET m_clientSocket;
	std::thread m_listenThread;
	int m_addressLength;
	struct sockaddr_in m_serverAddress;
	unsigned short m_IDNumber, m_sessionID;
	bool m_IsOnline, m_ThreadActive;
	int m_Latency;
	unsigned long m_PingTime;
	QueueT* m_networkMessageQueue;
	int m_nextQueueLocation, m_nextMessageForProcessing;
};

#endif