#include "ClientManager.h"

ClientManager::ClientManager() {
	login = false;
	sessionOn = false;
	roomOn = false;
	serverSocket = NULL;
}

void ClientManager::run()
{
	string parameter1;
	int partnerPort;
	int command;
	running = true;

	while (running)
	{
		command = serverSocket->readCommand();
		if (!command)
			continue;
		switch(command)
		{
			case SESSION_ESTABLISHED:
				udpChatSideB._sideB_name = serverSocket->readMsg();
				udpChatSideB._IP = serverSocket->readMsg();
				udpChatSideB._port = serverSocket->readCommand();

				partnerPort = serverSocket->readCommand();
				clientLinker = new ClientLinker(partnerPort);
				sessionOn = true;

				cout<<"You are in direct connection with user: "<<udpChatSideB._sideB_name<<endl;
				break;
			case SESSION_CREATE_REFUSED:
				parameter1 = serverSocket->readMsg();

				cout<<"Session has been denied because "<<parameter1 <<endl;
				break;
			case SESSION_CLOSED:
				udpChatSideB.Clean();
				sessionOn = false;
				delete(clientLinker);
				cout<<"the session is now terminated!"<<endl;
				break;
			case CHAT_ROOM_CREATE_DENIED:
				parameter1 = serverSocket->readMsg();
				cout << "room cannot be opened due to:" << parameter1 << endl;
				break;
			case CHAT_ROOM_CREATED:
				cout << "Chat room has been opened" << endl;
				break;
			case CHAT_ROOM_LEAVED:
				chatRoomLeaved();
				cout<<"your user is out of the room"<<endl;
				break;
			case CHAT_ROOM_LOGED_IN:
				partnerPort = serverSocket->readCommand();
				clientLinker = new ClientLinker(partnerPort);
				roomOn = true;
				cout << "You have logged in successfully! :)" << endl;
				break;
			case CHAT_ROOM_LOGED_IN_DENIED:
				cout << serverSocket->readMsg() << endl;
				break;
			case CHAT_ROOM_UPDATED:
				chatRoomUpdate();
				break;
			case CHAT_ROOM_CLOSED:
				cout<<"The room has been closed."<<endl;
				break;
			case CHAT_ROOM_UNCLOSED:
				cout<<"You cannot delete the room."<<endl;
				break;
			case LIST_CONNECTED_USERS:
				printConnectedUsers();
				break;
			case LIST_ROOMS:
				printRoomsList();
				break;
			case LIST_CONNECTED_USERS_IN_ROOM:
				printConnectedUsers();
				break;
			case LIST_USERS:
				printListUsers();
		}
	}
}

bool ClientManager::connectToServer(string ip, int port)
{
	if(serverSocket)
		return false;
	serverSocket = new TCPSocket(ip,port);
	return true;
}
void ClientManager::sign(string username,string password,int oper)
{
	int response;
	if(serverSocket)
	{
		serverSocket->writeCommand(oper);
		serverSocket->writeMsg(username);
		serverSocket->writeMsg(password);
		response = serverSocket->readCommand();

		if (response == SIGNUP_REQUEST_APPROVED)
		{
			cout<<"signed up was successful with the user: "<<username<<endl;
		}
		else if(oper==SIGNUP_REQUEST_DENIED)
		{
			cout<<"you failed to sign up"<<endl;
		}
	}
	else
	{
		cout<<"the server is not connected:("<<endl;
	}
}

void ClientManager::log(string username,string password,int oper)
{
	int response;
	if(serverSocket)
	{
		serverSocket->writeCommand(oper);
		serverSocket->writeMsg(username);
		serverSocket->writeMsg(password);
		response = serverSocket->readCommand();

		if(response == LOGIN_REQUEST_APPROVED)
		{
			_username = username;
			login = true;
			start();
			cout << "logged in was successful with the user: " << username << endl;
		}
		else if(oper==LOGIN_REQUEST)
		{
			cout<<"you enter wrong user or password "<<endl;
		}
	}
	else
	{
		cout<<"the server is not connected:("<<endl;
	}
}

bool ClientManager::openSession(string partnername)
{
	if (!serverSocket || !login || isInChat())
	{
		cout<<"You are not connected or not logged in"<<endl;
		return false;
	}
	serverSocket->writeCommand(SESSION_CREATE);
	serverSocket->writeMsg(partnername);

	return true;
}

void ClientManager::printCurrentInfo()
{
	if (serverSocket)
		cout<< "Connected to server "<<endl;
	else
		cout<< "NOT Connected to server "<<endl;
	if (login)
		cout<< "Logged in as  "<< _username<<endl;
	else
		cout<< "NOT logged in "<<endl;
	if (sessionOn)
		cout<< "In session "<<endl;
	else
		cout<< "NOT in session "<<endl;
	if (roomOn)
		cout<< "In chat room: "<<chatRoomName <<endl;
	else
		cout<< "NOT In chat room: "<<endl;
}

bool ClientManager::sendMsg(string msg)
{
	if(sessionOn)
	{
		clientLinker->send( string(">[")+_username+string("] ") + msg,udpChatSideB._IP,udpChatSideB._port);
		return true;
	}
	else if (roomOn)
	{
		std::vector<ChatSideB*>::iterator iter = chatUsers.begin();
		std::vector<ChatSideB*>::iterator enditer = chatUsers.end();

		while (iter != enditer)
		{
			clientLinker->send(string(">[")+_username+string("] ") + msg,(*iter)->_IP,(*iter)->_port);
			iter++;
		}
		return true;
	}
	return false;
}

bool ClientManager::createChatRoom(string name)
{
	if(!serverSocket || !login || isInChat())
		return false;
	serverSocket->writeCommand(CHAT_ROOM_CREATE);
	serverSocket->writeMsg(name);
	return true;
}

bool ClientManager::loginToChatRoom(string msg)
{
	if(isInChat() || !serverSocket || !login)
		return false;
	serverSocket->writeCommand(CHAT_ROOM_LOGIN);
	serverSocket->writeMsg(msg);
	return true;
}

void ClientManager::printConnectedUsers()
{
	int i;
	int numOfUsers;
	numOfUsers = serverSocket->readCommand();
	for(i=0 ; i<numOfUsers ; i++)
	{
		cout << " user: " << serverSocket->readMsg() << endl;
	}
	if(i == 0)
	{
		cout << "no one is connected" << endl;
	}
}

void ClientManager::printListUsers()
{
	int i;
	int numOfUsers;
	numOfUsers = serverSocket->readCommand();
	for(i=0 ; i<numOfUsers ; i++)
	{
		cout << " user: " << serverSocket->readMsg() << endl;
	}
	if(i == 0)
	{
		cout << "no one is connected"<< endl;
	}
}


void ClientManager::prinCtonnectedUsers()
{
	if (!serverSocket || !login)
	{
		cout << "You are not connected or not logged in"<<endl;
		return;
	}
	serverSocket->writeCommand(LIST_CONNECTED_USERS);
}

void ClientManager::listUsers()
{
	if (!serverSocket || !login)
	{
		cout << "You are not connected or not logged in"<<endl;
		return;
	}
	serverSocket->writeCommand(LIST_USERS);
}

void ClientManager::RoomsList()
{
	if (!serverSocket || !login)
	{
		cout<<"you are not connected or not logged in"<<endl;
		return;
	}
	serverSocket->writeCommand(LIST_ROOMS);
}

void ClientManager::listConnectedUsersInRoom(string roomName)
{
	if (!serverSocket || !login)
	{
		cout<<"you are not connected or not logged in"<<endl;
		return;
	}
	serverSocket->writeCommand(LIST_CONNECTED_USERS_IN_ROOM);
	serverSocket->writeMsg(roomName);
}

//bool ClientManager::closeSession()
//{
	//if (!sessionOn)
	//	return false;
	//serverSocket->writeCommand(SESSION_CLOSE);
	//return true;
//}

bool ClientManager::deleteChatRoom(string name)
{
	if (!serverSocket || !login)
	{
		cout<<"you are not connected or not logged in"<<endl;
		return false;
	}
	serverSocket->writeCommand(CHAT_ROOM_CLOSE);
	serverSocket->writeMsg(name);
	return true;
}

bool ClientManager::exitRoomOrCloseSession()
{
	if (roomOn)
	{
		serverSocket->writeCommand(CHAT_ROOM_EXIT);
	}
	else if (sessionOn)
	{
		serverSocket->writeCommand(SESSION_CLOSE);
	}
	else
	{
		return false;
	}

	return true;

}

void ClientManager::addRoomUser(string roomate, string IP, int port)
{
	ChatSideB *temp= new ChatSideB();
	temp->_sideB_name =roomate;
	temp->_IP = IP;
	temp->_port = port;
	chatUsers.push_back(temp);
}

void ClientManager::chatRoomLeaved()
{
	roomOn = false;
	clearRoomUsers();
	delete(clientLinker);
}

void ClientManager::clearRoomUsers()
{
	std::vector<ChatSideB*>::iterator iter = chatUsers.begin();
	std::vector<ChatSideB*>::iterator enditer = chatUsers.end();

	while (iter != enditer)
	{
		delete(*iter);
		iter++;
	}
	chatUsers.clear();
}

bool ClientManager::isInChat()
{
	return roomOn || sessionOn;
}

void ClientManager::exitAll()
{
	//closeSession();
	exitRoomOrCloseSession();
		serverSocket->writeCommand(EXIT);
	_username.clear();
	running = false;
	login = false;
	serverSocket = NULL;
}

void ClientManager::printRoomsList()
{
	int numOfRooms;
	numOfRooms = serverSocket->readCommand();

	if(numOfRooms > 0)
		cout << "Rooms: "<<endl;
	for(int i = 0 ; i<numOfRooms;i++)
	{
		cout << "Room name " << serverSocket->readMsg() << endl;
	}
}

void ClientManager::chatRoomUpdate()
{
	roomOn = true;
	clearRoomUsers();
	chatRoomName = serverSocket->readMsg();
	int countofmemebers = serverSocket->readCommand();
	for (int i = 0 ; i< countofmemebers;i++)
	{
		string user = serverSocket->readMsg();
		string ip = serverSocket->readMsg();
		int port = serverSocket->readCommand();
		addRoomUser(user,ip,port);
	}
	cout<<"Chat room "<< chatRoomName<<" updated"<<endl;
}

ClientManager::~ClientManager() {
	running = false;
}
