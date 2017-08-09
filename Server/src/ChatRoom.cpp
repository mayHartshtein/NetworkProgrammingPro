#include "ChatRoom.h"

ChatRoom::ChatRoom(User* owner, string chatRoom) {
	pthread_mutex_init(&mutx_roomUsersVec,NULL);
	_roomOwner = owner;
	_chatRoomName = chatRoom;
}

void ChatRoom::updateusers()
{
	pthread_mutex_lock(&mutx_roomUsersVec);
	for(vector<User*>::iterator iter = charRoomUsers.begin(); iter != charRoomUsers.end(); iter++)
	{
		(*iter)->writeCommand(CHAT_ROOM_UPDATED);
		(*iter)->writeMsg(_chatRoomName);
		(*iter)->writeCommand(charRoomUsers.size());

		for(vector<User*>::iterator iter2 = charRoomUsers.begin(); iter2 != charRoomUsers.end(); iter2++)
		{
			(*iter)->writeMsg((*iter2)->getusername());
			(*iter)->writeMsg((*iter2)->getIP());
			(*iter)->writeCommand((*iter2)->getport());
		}
	}
	pthread_mutex_unlock(&mutx_roomUsersVec);
}

void ChatRoom::printUsers()
{
	pthread_mutex_lock(&mutx_roomUsersVec);
	for(vector<User*>::iterator iter = charRoomUsers.begin(); iter != charRoomUsers.end(); iter++)
	{
		cout<<(*iter)->getusername()<<endl;
	}
	pthread_mutex_unlock(&mutx_roomUsersVec);
}

// send list of users in the same
int ChatRoom::sendUserList(User *sendto)
{
	int numOfUsers = 0;

	pthread_mutex_lock(&mutx_roomUsersVec);

	sendto->writeCommand(charRoomUsers.size());
	for(vector<User*>::iterator iter = charRoomUsers.begin(); iter != charRoomUsers.end(); iter++)
	{
		sendto->writeMsg((*iter)->getusername());
		numOfUsers++;
	}

	pthread_mutex_unlock(&mutx_roomUsersVec);
	return numOfUsers;
}

// add user to room
bool ChatRoom::addUser(User* userToAdd)
{
	bool exist = false;

	// check if exist
	pthread_mutex_lock(&mutx_roomUsersVec);

	for(vector<User*>::iterator iter = charRoomUsers.begin(); iter != charRoomUsers.end(); iter++)
	{
		if ((*iter) == userToAdd)
		{
			pthread_mutex_unlock(&mutx_roomUsersVec);
			exist = true;
		}

	}
	pthread_mutex_unlock(&mutx_roomUsersVec);


	if(exist)
	{
		cout << "User already exist" <<endl;
		return false;
	}

	pthread_mutex_lock(&mutx_roomUsersVec);
	charRoomUsers.push_back(userToAdd);
	pthread_mutex_unlock(&mutx_roomUsersVec);
	updateusers();

	return true;
}

// return room owner
User* ChatRoom::getOwner()
{
	return _roomOwner;
}

// log off a user from the server
bool ChatRoom::logOffUser(User *usertologof)
{
	bool exist = false;

	// check if exist
	pthread_mutex_lock(&mutx_roomUsersVec);

	for(vector<User*>::iterator iter = charRoomUsers.begin(); iter != charRoomUsers.end(); iter++)
	{
		if ((*iter) == usertologof)
		{
			pthread_mutex_unlock(&mutx_roomUsersVec);
			exist = true;
		}

	}
	pthread_mutex_unlock(&mutx_roomUsersVec);


	if(!exist){
		return false;
	}

	pthread_mutex_lock(&mutx_roomUsersVec);
	for(vector<User*>::iterator iter = charRoomUsers.begin(); iter != charRoomUsers.end(); iter++)
		{
		if ((*iter) == usertologof)
		{
			charRoomUsers.erase(iter);
			break;
		}
	}
	pthread_mutex_unlock(&mutx_roomUsersVec);
	updateusers();
	return true;
}

ChatRoom::~ChatRoom() {

	pthread_mutex_lock(&mutx_roomUsersVec);
	for(vector<User*>::iterator iter = charRoomUsers.begin(); iter != charRoomUsers.end(); iter++)
	{
		(*iter)->disconnectFromChatRom(true);
	}
	pthread_mutex_unlock(&mutx_roomUsersVec);

}
