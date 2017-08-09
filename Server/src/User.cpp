#include "User.h"

User::User(string name, TCPSocket* sock)
{
	_sock = sock;
	string tempport;
	istringstream liness( _sock->destIpAndPort() );
	getline( liness, _ip, ':' );
	getline( liness, tempport,  ':' );
	_port= atoi(tempport.c_str());

	_inChatRoom = false;
	_inSession = false;
	_name = name;
}

TCPSocket* User::getSocket()
{
	return _sock;
}

bool User::inChat()
{
	return inChatRoom() || inSession();
}

bool User::inChatRoom()
{
	return _inChatRoom;
}

bool User::inSession()
{
	return _inSession;
}

void User::loginUsertoSession(User* partner)
{
	_inSession = true;
	_Chatpartner = partner;

}

void User::disconnectFromChatRom(bool fromchatroom)
{
	if(inChatRoom())
	{
		if (!fromchatroom)
			_chatroom->logOffUser(this);
		writeCommand(CHAT_ROOM_LEAVED);
		_chatroom = NULL;
		_inChatRoom = false;
	}

}

void User::loginUserToChatRoom(ChatRoom* logtoroom)
{
	_inChatRoom = true;
	_chatroom = logtoroom;
}

bool User::closeSession(bool isinitiating)
{
	if (!inSession())
		return true;
	if (isinitiating)
		_Chatpartner->closeSession(false);

	writeCommand(SESSION_CLOSED);
	_inSession = false;
	_Chatpartner = NULL;
	return true;
}

int User::readCommand()
{
	return _sock->readCommand();
}
string User::readMsg()
{
	return _sock->readMsg();
}
void User::writeMsg(string msg)
{
	_sock->writeMsg(msg);
}
void User::writeCommand(int command)
{
	_sock->writeCommand(command);
}

string User::getDestandport()
{
	return _sock->destIpAndPort();
}

string User::getusername()
{
	return _name;
}

string User::getIP()
{
	return _ip;
}

int User::getport()
{
	return _port;
}

ChatRoom* User::getChatRoom()
{
	return _chatroom;
}

User::~User() {
	delete(_sock);
}
