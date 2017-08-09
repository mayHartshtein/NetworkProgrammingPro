#ifndef CHATROOM_H_
#define CHATROOM_H_

#include "User.h"
#include <vector>

class User;

class ChatRoom {
	string _chatRoomName;
	vector<User*> charRoomUsers;
	User* _roomOwner;
	pthread_mutex_t mutx_roomUsersVec;
	void updateusers();

public:
	ChatRoom(User* owner, string chatroom);
	bool  logOffUser(User *usertologof);
	bool  addUser(User* userToAdd);
	User* getOwner();
	int   sendUserList(User *sendto);
	void  printUsers();
	virtual ~ChatRoom();
};

#endif /* CHATROOM_H_ */
