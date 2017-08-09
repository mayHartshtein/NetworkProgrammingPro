#include "ServerManager.h"
#include "MultipleTCPSocketListener.h"

ServerManager::ServerManager() {

	pthread_mutex_init(&users_mutx,NULL);
	pthread_mutex_init(&chatroom_mutx,NULL);
	multiUserListener = NULL;
	running = true;

	start();
	cout<<"server manager is now up"<<endl;
}


void ServerManager::run()
{
	running = true;
		User* currUser = NULL;
		multiUserListener = NULL;
		long command = 0;
		string msg;
		while (running)
		{
			// Create listener
			if (multiUserListener != NULL)
			{
				delete(multiUserListener);
			}
			multiUserListener = new MultipleTCPSocketsListener();


			//---------------------------------lock--------------------------------
			pthread_mutex_lock(&users_mutx);

			// convert User to Socket before adding to list
			map<string,TCPSocket*> toSocket;
			for (map<string,User*>::iterator biter = usersMap.begin(); biter != usersMap.end(); biter++)
			{
				toSocket[biter->first] = (biter->second->getSocket());
			}

			// Add socket
			multiUserListener->addSockets(toSocket);

			//----------------------------free lock------------------------------
			pthread_mutex_unlock(&users_mutx);
			//-------------------------------------------------------------------



			// listen to socket
			TCPSocket* readyPeer = multiUserListener->listenToSocket(2);
			if (!readyPeer){
				continue;
			}

			// ------------------------- lock --------------------------------------
			pthread_mutex_lock(&users_mutx);

			//found user from tcpsocket list
			for (map<string,User*>::iterator iter = usersMap.begin(); iter != usersMap.end(); iter++)
			{
				if (readyPeer == iter->second->getSocket())
				{
					currUser = iter->second;
					break;
				}
			}

			// ----------------------free lock---------------------------------------
			pthread_mutex_unlock(&users_mutx);
			//-----------------------------------------------------------------------

			// READ command from the user
			command = currUser->readCommand();

			switch(command)
			{
			case 0:
				exitServer(currUser);
				break;
			case SESSION_CREATE:
				msg = currUser->readMsg();
				if (!usersMap[msg])
				{
					currUser->writeCommand(SESSION_CREATE_REFUSED);
					currUser->writeMsg(string("there is no  such user"));
					break;
				}
				else if (usersMap[msg]->inChat())
				{
					currUser->writeCommand(SESSION_CREATE_REFUSED);
					currUser->writeMsg(string("the wanted user in chat"));
					break;
				}
				createSession(currUser,usersMap[msg]);
				cout<<"Session was created between: "<<currUser->getusername()<<" AND "<<msg<<endl;
				break;
			case EXIT:
				exitServer(currUser);
				break;
			case SESSION_CLOSE:
				currUser->closeSession(true);
				break;
			case CHAT_ROOM_CREATE:
				createChatRoom(currUser);
				break;
			case CHAT_ROOM_CLOSE:
				deleteChatRoom(currUser);
				break;
			case CHAT_ROOM_LOGIN:
				loginChatRoom(currUser);
				break;
			case CHAT_ROOM_EXIT:
				currUser->disconnectFromChatRom(false);
				break;
			case LIST_CONNECTED_USERS:
				getListConnectedUsers(currUser);
				break;
			case LIST_ROOMS:
				getListRooms(currUser);
			break;
			case LIST_CONNECTED_USERS_IN_ROOM:
				getListChatUsers(currUser);
				break;
			case LIST_USERS:
				getListUsers(currUser);
				break;
			}
		}
	}

// add user to users list
bool ServerManager::addUser(TCPSocket* userSocket, string LoginUserName)
{
	bool added = true;
	User* userToAdd = new User(LoginUserName,userSocket);

	int numBefor = this->getListConnectedUsers();
	pthread_mutex_lock(&users_mutx);
	usersMap.insert(pair<string,User*>(LoginUserName,userToAdd));
	pthread_mutex_unlock(&users_mutx);
	cout << "user " << LoginUserName << " has connected" << endl;

//	if( this->getListConnectedUsers() <= numBefor)
//	{
//		added = false;
//	}
	return added;
}

// Print List of connected users
int ServerManager::getListConnectedUsers()
{
	cout<<"The Connected users are:"<<endl;

		map<string,User*>::iterator iter ;

		string name;
		int count = 0;
		for (iter = usersMap.begin() ; iter != usersMap.end();iter++)
			{
				name = iter->first;
				printToSreen(name);
				count++;
			}
		return count;
}

// Send  List of connected users to the asking client
int ServerManager::getListConnectedUsers(User *client)
{

	client->writeCommand(LIST_CONNECTED_USERS);
	client->writeCommand(usersMap.size());

	map<string,User*>::iterator iter ;

	string name;
	int count = 0;
	for (iter = usersMap.begin() ; iter != usersMap.end();iter++)
	{
		name = iter->first;
		client->writeMsg( name);
		count++;
	}

	return count;
}

// get list of all open sessions
void ServerManager::getListSessions()
{
	cout<<"All the connected users that in Session:"<<endl;
	std::map<string,User*>::iterator iter;

	string name;
	for (iter = usersMap.begin() ; iter != usersMap.end();iter++)
	{
		if(iter->second->inSession())
		{
			name = iter->first;
			printToSreen(name);
		}
	}
}

// Print list of all chat rooms
void ServerManager::getListRooms()
{

	cout<<"the rooms list:"<<endl;
	this->readfromChatRoom(NULL);

}

// Send  list of chat rooms to the asking client
void ServerManager::getListRooms(User *client)
{
	client->writeCommand(LIST_ROOMS);
	client->writeCommand(chatRoomsMap.size());

	this->readfromChatRoom(client);
}

// Print list of users in a specific room
int ServerManager::getListChatUsers(string ChatRoomName)
{
	cout<<"Users list in Room:"<<endl;
		int numOfUsers = 0;
		for (map<string,ChatRoom*>::iterator iter = chatRoomsMap.begin(); iter !=  chatRoomsMap.end();iter++)
		{
			if (ChatRoomName == iter->first){
				iter->second->printUsers();
				numOfUsers++;
			}
		}
		return numOfUsers;
}

// Send list of users in a specific room to the asking client
int ServerManager::getListChatUsers(User *client)
{
	int numOfUsers = 0;
	string ChatRoomName = client->readMsg();
	for (map<string,ChatRoom*>::iterator iter = chatRoomsMap.begin(); iter !=  chatRoomsMap.end();iter++)
	{
		if (ChatRoomName == iter->first)
		{
			client->writeCommand(LIST_CONNECTED_USERS_IN_ROOM);
			(iter)->second->sendUserList(client);
			break;
		}
	}
	return numOfUsers;
}

// Swnd List of users from login file to asking client
void ServerManager::getListUsers(User *client)
{
	int numOfusers = 0;
	numOfusers = numOfUsersFromFile();

	client->writeCommand(LIST_USERS);
	client->writeCommand(numOfusers -1);

	//cout << "the users:"<< endl;
	if (client != NULL)
		readFromFile(client);
	else
		readFromFile(NULL);

}

// Print List of users from login file
void ServerManager::getListUsers()
{
	cout << "the users:"<< endl;
	readFromFile(NULL);}

// exit from the server
void ServerManager::exitServer(User* clientName)
{
	clientName->closeSession(true);
	clientName->disconnectFromChatRom(false);

	cout<<"the Client  : "<< clientName->getusername() << " was  disconnect" <<endl;

	pthread_mutex_lock(&users_mutx);
	usersMap.erase(clientName->getusername());
	pthread_mutex_unlock(&users_mutx);
}

// Create a session between two users
void ServerManager::createSession(User* fromUser, User* toUser) {
	// login the two users
	toUser->loginUsertoSession(fromUser);
	fromUser->loginUsertoSession(toUser);

	// send communication details
	toUser->writeCommand(SESSION_ESTABLISHED);
	toUser->writeMsg(fromUser->getusername());
	toUser->writeMsg(fromUser->getIP());
	toUser->writeCommand(fromUser->getport());
	toUser->writeCommand(toUser->getport());

	fromUser->writeCommand(SESSION_ESTABLISHED);
	fromUser->writeMsg(toUser->getusername());
	fromUser->writeMsg(toUser->getIP());
	fromUser->writeCommand(toUser->getport());
	fromUser->writeCommand(fromUser->getport());

}


// Create new chat room
void ServerManager::createChatRoom(User* creator)
{
	string msg;
	bool exist = false;
	msg = creator->readMsg();

	// Checking that the name does not exist

	pthread_mutex_lock(&chatroom_mutx);
	for (map<string,ChatRoom*>::iterator iter = chatRoomsMap.begin(); iter !=  chatRoomsMap.end();iter++)
	{
		if (iter->first == msg)
		{
			pthread_mutex_unlock(&chatroom_mutx);
			exist = true;
		}
	}
	pthread_mutex_unlock(&chatroom_mutx);


	if (exist)
	{
		creator->writeCommand(CHAT_ROOM_CREATE_DENIED);
		creator->writeMsg(string("Chat Room name already exists"));
		return;
	}

	//Add new caht room
	pthread_mutex_lock(&chatroom_mutx);
	chatRoomsMap[msg] = new ChatRoom(creator, msg);
	pthread_mutex_unlock(&chatroom_mutx);
	creator->writeCommand(CHAT_ROOM_CREATED);
	cout << "Room : "<< msg <<" was created" << endl;
}


// delete chat room
void ServerManager::deleteChatRoom(User* creator)
{
	string msg;
	msg = creator->readMsg();

	bool exist = false;

	pthread_mutex_lock(&chatroom_mutx);
	for (map<string,ChatRoom*>::iterator iter = chatRoomsMap.begin(); iter !=  chatRoomsMap.end();iter++)

	{
		if (iter->first == msg)
		{
			pthread_mutex_unlock(&chatroom_mutx);
			exist = true;
		}
	}
	pthread_mutex_unlock(&chatroom_mutx);


	if(!exist){
		return;
	}

	pthread_mutex_lock(&chatroom_mutx);
	if(chatRoomsMap[msg]->getOwner()->getusername() == creator->getusername())
	{
		delete(chatRoomsMap[msg]);
		creator->writeCommand(CHAT_ROOM_CLOSED);
		chatRoomsMap.erase(msg);
	}
	else
	{
		creator->writeCommand(CHAT_ROOM_UNCLOSED);
	}
	pthread_mutex_unlock(&chatroom_mutx);


}


// login chat room
void ServerManager::loginChatRoom(User* loginUser)
{
	string roomname = loginUser->readMsg();
	bool exist = false;
	pthread_mutex_lock(&chatroom_mutx);
	for (map<string,ChatRoom*>::iterator iter = chatRoomsMap.begin(); iter !=  chatRoomsMap.end();iter++)
	{
		if (iter->first == roomname)
		{
			pthread_mutex_unlock(&chatroom_mutx);
			exist = true;
		}
	}
	pthread_mutex_unlock(&chatroom_mutx);


	if (!exist)
	{
		loginUser->writeCommand(CHAT_ROOM_LOGED_IN_DENIED);
		loginUser->writeMsg(string("Room does not exist"));
		return;
	}

	loginUser->loginUserToChatRoom(chatRoomsMap[roomname]);
	if (chatRoomsMap[roomname]->addUser(loginUser))
	{
		loginUser->writeCommand(CHAT_ROOM_LOGED_IN);
		loginUser->writeCommand(loginUser->getport());
	}
	else
	{
		loginUser->writeCommand(CHAT_ROOM_LOGED_IN_DENIED);
		loginUser->writeMsg(string("you are already logged in"));
	}


}

int ServerManager::numOfUsersFromFile(){
	string line;
	fstream loginFile;
	string userFromFile;
	int counter = 0;

		loginFile.open(_loginfile,ios::in|ios::out|ios::binary);

	    while ( !loginFile.eof() )
	    {
	      getline (loginFile,line);
	      counter++;

	    }
	    loginFile.close();
	 return counter;
}

void ServerManager::readFromFile(User *clientName)
{
	string line;
		fstream loginFile;
		string userFromFile;
		loginFile.open(_loginfile,ios::in|ios::out|ios::binary);
		if (loginFile.is_open()){

			while ( !loginFile.eof() )
			{
			  getline (loginFile,line);
			  istringstream liness( line );
			  getline( liness, userFromFile, '-' );
			  if (clientName != NULL)
			  {
				  clientName->writeMsg(userFromFile);
				  //printToSreen(userFromFile);

			  }
			  else
			  {
				  printToSreen(userFromFile);
			  }

			}
			loginFile.close();
		}
		else
		{
			 printToSreen("Error - could not open the file");
		}

}

void ServerManager::printToSreen(string msgToScreen){
    cout << msgToScreen << endl;
}

void ServerManager::readfromChatRoom(User *clientName)
{
	string name;
	pthread_mutex_lock(&chatroom_mutx);
		for (map<string,ChatRoom*>::iterator iter = chatRoomsMap.begin(); iter !=  chatRoomsMap.end();iter++)
		{
			name = iter->first;
			if (clientName != NULL)
			{
			clientName->writeMsg(name);
			}
			else
			{
				printToSreen(name);
			}

		}
		pthread_mutex_unlock(&chatroom_mutx);
}


ServerManager::~ServerManager() {
	running = false;
	waitForThread();
}
