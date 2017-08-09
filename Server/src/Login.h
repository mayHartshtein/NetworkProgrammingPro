#ifndef LOGIN_H_
#define LOGIN_H_

#include "MThread.h"
#include <map>
#include <string>
#include "TCPSocket.h"
#include "MultipleTCPSocketListener.h"
#include "TCPMessengerProtocol.h"
#include "ServerManager.h"

using namespace std;

class Login: public MThread {
	const char* _loginfile = "connections.txt";
	map<string, TCPSocket*> peers;
	bool running;
	MultipleTCPSocketsListener *listener;
	pthread_mutex_t peers_mutx;
	ServerManager* _serverManager;

	bool login(string userName,string password);
	bool signUp(string userName,string password);

public:

	Login(ServerManager* _serverManager);
	void run();
	void addPeer(TCPSocket* peer);
	virtual ~Login();
};



class Connection: public MThread
{
	Login* registration;
	TCPSocket * clientSocket;
	bool running;

public:

	Connection(Login* registration);
	void run();
	virtual ~Connection();
};

#endif /* LOGIN_H_ */
