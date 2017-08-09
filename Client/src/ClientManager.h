#ifndef CLIENTMANAGER_H_
#define CLIENTMANAGER_H_

//#include "ServerLinker.h"
#include "MThread.h"
#include "ClientLinker.h"
#include <vector>
#include "TCPSocket.h"
#include "TCPMessengerProtocol.h"

class ChatSideB
{
public:
	string _sideB_name;
	string _IP;
	int _port;
	void Clean()
	{
		_sideB_name.clear();
		_IP.clear();
		_port = 0;
	}
};

class ClientManager : public MThread{
//	ServerLinker* serverLinker;
	TCPSocket* serverSocket;
	bool running;
	ChatSideB udpChatSideB;
	string _username;
	vector<ChatSideB*> chatUsers;
	string chatRoomName;
	ClientLinker *clientLinker;
	bool sessionOn;
	bool roomOn;
	bool login;

	void run();

	void clearRoomUsers();
	void addRoomUser(string roomate, string IP, int port);

public:
	bool connectToServer(string ip,int port);
	void sign(string username,string password,int oper);
	void log(string username,string password,int oper);
	bool openSession(string chatSideBName);
	void printCurrentInfo();
	bool sendMsg(string msg);

	bool createChatRoom(string name);
	bool loginToChatRoom(string msg);

	void prinCtonnectedUsers();
	void printConnectedUsers();
	void printListUsers();

	void RoomsList();
	void listConnectedUsersInRoom(string roomName);
	void listUsers();

	//bool closeSession();
	bool exitRoomOrCloseSession();
	bool deleteChatRoom(string msg);

	//bool isConnectedToServer();
	bool isInChat();

	void exitAll();
	ClientManager();
	virtual ~ClientManager();













	//handling server responses
	void chatRoomLeaved();
	void chatRoomUpdate();





	void printRoomsList();





};

#endif /* CLIENTMANAGER_H_ */
