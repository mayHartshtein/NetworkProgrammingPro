#ifndef SERVERMANAGER_H_
#define SERVERMANAGER_H_

#include "MThread.h"
#include "TCPSocket.h"
#include <map>
#include "MultipleTCPSocketListener.h"
#include "User.h"
#include "TCPMessengerProtocol.h"

class User;

class ServerManager: public MThread {
const char* _loginfile = "connections.txt";
MultipleTCPSocketsListener *multiUserListener;
pthread_mutex_t users_mutx;
pthread_mutex_t chatroom_mutx;
map<string, User*> usersMap;
map<string, ChatRoom*> chatRoomsMap;
bool running;

bool createSession(string userToChatWith);
bool createCR(string chatRoomName);
bool closeCR(string chatRoomName);
bool loginToChatRoom(string chatRoomName);
void createSession(User* initiatingUser, User* targetUser);
bool chatRoomExists(string chatRoomName);

map<string,User*>::iterator getBeginIter();
map<string,User*>::iterator getEndIter();

public:

	ServerManager();
	void run();
	bool addUser(TCPSocket*  userSocket, string LoginUserName);
	void exitServer(User* clientName);
	void readFromFile(User *clientName);
	void readfromChatRoom(User *clientName);
	void readfromUsers(User *clientName);
	void printToSreen(string msgToScreen);
	int  numOfUsersFromFile();
	int  numOfUserFromList();
	int  getListConnectedUsers();
	int  getListConnectedUsers(User *client);
	void getListSessions();
	void getListRooms();
	void getListRooms(User *clientName);
	int  getListChatUsers(string ChatRoomName);
	int  getListChatUsers(User *clientName);
	void getListUsers();
	void getListUsers(User *clientName);

	void createChatRoom(User* creator);
	void deleteChatRoom(User* creator);
	void loginChatRoom(User* creator);

	virtual ~ServerManager();

};

#endif /* SERVERMANAGER_H_ */
