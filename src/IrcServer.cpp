/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 11:14:16 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/11 12:46:21 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcServer.hpp"

/**
 * @brief Construct a new Parameterized Irc Server:: Irc Server object
 * @param args Arguments
*/
IrcServer::IrcServer(const std::string args[])
{
	std::istringstream iss(args[1]);
	if (!(iss >> _port))
	{
		std::cerr << "Erro: Porta inválida\n";
		//std::exit(EXIT_FAILURE);
	}
	_pwd = args[2];
	startServer();
}

/**
 * @brief Destroy the Irc Server:: Irc Server object
*/
IrcServer::~IrcServer()
{
}

/**
 * @brief Copy the IrcServer object
 * 
 * @param rhs IrcServer object to copy
 * @return IrcServer& IrcServer object
*/
IrcServer &IrcServer::operator=(const IrcServer &rhs)
{
	_port = rhs._port;
	_pwd = rhs._pwd;
	return (*this);
}

/**
 * @brief Set the Client object
 * 
 * @param client Client
*/
void IrcServer::addChannel(Channel *channel)
{
	_channels.push_back(channel);

}

/**
 * @brief Set the Client object
 * 
 * @param client Client
*/
void IrcServer::addUser(Client *client)
{
	_users.push_back(client);
}

/**
 * @brief Starts the IRC server
 * 
*/
void IrcServer::startServer()
{
	/*
	* Create a socket
	* AF_INET: IPv4
	* SOCK_STREAM: TCP
	* 0: Protocol (IP)
	*/
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(_socket < 0)
	{
		std::cerr << "Error: Socket creation failed" << std::endl;
		//exit(1);
	}
	int opt = 1;
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
	{
		std::cerr << "Error: Setsockopt failed" << std::endl;
		close(_socket);
		//exit(1);
	}
	// Bind the socket to an IP / port
	// sockaddr_in is a structure containing an internet address
	// sin_family: AF_INET - IPv4
	// sin_port: Port number (htons to convert to big endian)
	// sin_addr: IP address (INADDR_ANY to bind to all local interfaces)
	memset(&_server_addr, 0, sizeof(_server_addr));
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(_port);
	_server_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(_socket, (sockaddr *)&_server_addr, sizeof(_server_addr)))
	{
		std::cerr << "Error: Bind failed" << std::endl;
		close(_socket);
		throw std::exception();
		//exit(1);
	}
	if (listen(_socket, 5) < 0)
	{
		std::cerr << "Error: Listen failed" << std::endl;
		close(_socket);
		throw std::exception();
		//exit(1);
	}
	std::cout << "Server started on port: " << _port << std::endl;
	Channel *channel = new Channel("general");
	//TODO: handle alloc error
	addChannel(channel);
}

void IrcServer::run()
{
	int client_fd = 0;
	struct pollfd pfd;
	pfd.fd = _socket;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_poll_fds.push_back(pfd);
	while (1)
	{
		int event_count = poll(_poll_fds.data(), _poll_fds.size(), -1);
        if (event_count == -1)
        {
			std::cerr << "Error: poll() failed" << std::endl;
            break;
        }
        for (size_t i = 0; i < _poll_fds.size(); i++)
        {
			if (_poll_fds[i].revents & POLLIN) 
            {
                if (_poll_fds[i].fd == _socket)
				{
					Client *newClient = new Client(*this);
					//TODO: handle alloc error
					addUser(newClient);
					client_fd = _users.back()->acceptClient(_poll_fds[i].fd);
					if (client_fd < 0)
					{
						std::cerr << "Error accepting client" << std::endl;
						// return;
					}
					_users.back()->setFd(client_fd);
					struct pollfd newPoll;
					newPoll.fd = client_fd;
					newPoll.events = POLLIN;
					newPoll.revents = 0;
					_poll_fds.push_back(newPoll);
					send(client_fd, "NICK:\n", 5, 0);
					_users.back()->validateUser(client_fd);
				}
                else
				{
					getUserFd(_poll_fds[i].fd)->handleClientMessage(_poll_fds[i].fd);
				}
            }	
		}
	}
}

/*---------------------------- COMMANDS ---------------------------- */

/**
 * @brief Command to change the nickname of the user
 * @param client_fd File descriptor of the client
*/
void IrcServer::nickCommand(int client_fd, std::string restOfCommand)
{
	std::string inputNick = restOfCommand;
	inputNick = clean_input(inputNick, SPACES);
	std::string msg;
	if(inputNick.empty())
	{
		send(client_fd, "Invalid nickname\n", 17, 0);
	}
	//TODO: Check if nickname is already in use and it's uppercase and lowercase letters. Ex: DOT it's the same nick as dot
	getUserFd(client_fd)->setUser(inputNick, getUserFd(client_fd)->getPass());
	msg = "User " + inputNick + " changed nickname\n";
	send(client_fd, msg.c_str(), msg.length(), 0);
	std::cout << msg;
}

/**
 * @brief Command to join a channel
 * @param client_fd File descriptor of the client
*/
void IrcServer::joinCommand(int client_fd, std::string restOfCommand)
{
	std::istringstream iss(restOfCommand);
	std::string channelName;
	std::string passwd;
	
	iss >> channelName >> passwd;
	channelName = clean_input(channelName, SPACES);
	bool channelExists = false;
	std::string msg;
	if(channelName.empty())
	{
		send(client_fd, "Invalid channel\n", 16, 0);
		return ;
	}
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if ((_channels[i]->getName() == channelName) && _user->getChannel() == NULL)
		{
			channelExists = true;
			if (_channels[i]->getInviteOnly() && !_user->getOperator() && !_channels[i]->getInvitedUser(client_fd))
			{
				send(client_fd, "Channel is invite only\n", 24, 0);
				break;
			}
			if (_channels[i]->getLimit() && _channels[i]->getUsers().size() >= _channels[i]->getLimit())
			{
				send(client_fd, "Channel is full\n", 16, 0);
				break;
			}
			for (size_t j = 0; j < _channels[i]->getUsers().size(); j++)
			{
				if (_channels[i]->getUsers()[j]->getNick() != _user->getNick())
				{
					if (_channels[i]->getPassword().empty())
					{
						_channels[i]->addUser(_user);
						_user->setChannel(_channels[i]);
						msg = "User " + _user->getNick() + " added to channel: " + _channels[i]->getName() + "\n";
						send(client_fd, msg.c_str(), msg.length(), 0);
						std::cout << msg;
						break;
					}
					else if (_channels[i]->getPassword() != passwd)
					{
						send(client_fd, "Channel has password\n", 21, 0);
						break;
					}
					_channels[i]->addUser(_user);
					_user->setChannel(_channels[i]);
					msg = "User " + _user->getNick() + " added to channel: " + _channels[i]->getName() + "\n";
					send(client_fd, msg.c_str(), msg.length(), 0);
					std::cout << msg;
					break;
				}
			}
			break;
		}
	}
	if (!channelExists)
	{
		Channel *channel = new Channel(channelName);
		//TODO:handle alloc error
		addChannel(channel);
		_channels[_channels.size() - 1]->addUser(_user);
		_user->setChannel(_channels[_channels.size() - 1]);
		msg = "\nChannel " + _channels[_channels.size() - 1]->getName() + " created and user " + _user->getNick() + " added\n\n";
		_user->setOperator(true);
		std::cout << msg;
	}
	if (_user->getChannel())
	{
		send(client_fd, "\n          Channel: ", 20, 0);
		send(client_fd, _user->getChannel()->getName().c_str(), _user->getChannel()->getName().size(), 0);
		send(client_fd, "\n\n", 1, 0);
	}
	//TODO:Check if channel exists and if it has pwd
}

/**
 * @brief Command to leave a channel
 * @param client_fd File descriptor of the client
*/
void IrcServer::leaveCommand(int client_fd)
{
	if (_user->getChannel())
	{
		sendClientMsg(client_fd, "User left channel\n", 0);
		_user->getChannel()->removeUser(_user);
		_user->setChannel(NULL);
	}
	else
		send(client_fd, "You are not in a channel\n", 25, 0);
}

/**
 * @brief Command to list all channels
 * @param client_fd File descriptor of the client
*/
void IrcServer::listCommand(int client_fd, std::string restOfCommand)
{
	if(restOfCommand.empty())
	{
		for (size_t i = 0; i < _channels.size(); i++)
		{
			send(client_fd, "Channel: ", 9, 0);
			send(client_fd, _channels[i]->getName().c_str(), _channels[i]->getName().length(), 0);
			send(client_fd, "\n", 1, 0);
		}
	}
	else
	{
		//TODO: List all channels with the full/part name of the channel (Deal with wildcards?)
	}
}

void IrcServer::usersCommand(int client_fd)
{
	size_t i = 0;
	std::string msg;
	for (;i < _channels.size(); i++)
	{
		if (_channels[i]->getUserFd(client_fd))
			break;
	}
	if (i == _channels.size() || !_user->getChannel())
	{
		send(client_fd, "You are not in a channel\n", 25, 0);
		return ;
	}
	msg = "Channel: " + _channels[i]->getName();
	send(client_fd, msg.c_str(), msg.length(), 0);
	for (size_t j = 0; j < _channels[i]->getUsers().size(); j++)
	{
		msg = "\nUser: " + _channels[i]->getUsers()[j]->getNick();
		send(client_fd, msg.c_str(), msg.length(), 0);
	}
	//TODO: Pretty print of the users in the channel
}

void IrcServer::exitCommand(int client_fd)
{
	(void)client_fd;
	_user->getChannel()->removeUser(_user);
	_user->setChannel(NULL);
	//TODO: Close connection with the client
}

void IrcServer::kickCommand(int client_fd, std::string restOfCommand)
{
	restOfCommand = clean_input(restOfCommand, SPACES);
	if(_user->getOperator() == false)
	{
		send(client_fd, "Not allowed", 11, 0);
		return ;
	}
	std::string inputNick = restOfCommand;
	if(inputNick.empty())
	{
		send(client_fd, "Invalid nickname\n", 17, 0);
	}
	if(_user->getChannel())
	{
		for (size_t i = 0; i < _user->getChannel()->getUsers().size(); i++)
		{
			if (_user->getChannel()->getUsers()[i]->getNick() == inputNick)
			{
				_user->getChannel()->getUsers()[i]->setOperator(false);
				_user->getChannel()->getUsers()[i]->setChannel(NULL);
				_user->getChannel()->removeInvitedUser(_user->getChannel()->getUsers()[i]);
				send(_user->getChannel()->getUsers()[i]->getFd(), "You have been kicked from your channel\n", 39, 0);
				_user->getChannel()->removeUser(_user->getChannel()->getUsers()[i]);
				break;
			}
		}
	}
	else
		send(client_fd, "No user found\n", 14, 0);
}

void IrcServer::inviteCommand(int client_fd, std::string restOfCommand)
{
	std::istringstream iss(restOfCommand);
	std::string nickname;
	std::string channelName;
	
	iss >> nickname >> channelName;
	if (nickname.empty() || channelName.empty())
	{
		send(client_fd, "Invalid INVITE command\n", 24, 0);
		return;
	}
	if (_user->getChannel()->getName() != channelName)
	{
		send(client_fd, "You are not in the channel\n", 25, 0);
		return ;
	}
	if (_user->getOperator() == false)
	{
		send(client_fd, "Not allowed", 11, 0);
		return ;
	}
	if (!getChannelByName(channelName))
	{
		send(client_fd, "Channel not found\n", 18, 0);
		return ;
	}
	if (!getUserByNick(nickname))
	{
		send(client_fd, "User not found\n", 15, 0);
		return ;
	}
	_user->getChannel()->addInvitedUser(getUserByNick(nickname));
	send(client_fd, "User invited to channel\n", 24, 0);
	std::string inputNick = restOfCommand;
	if(inputNick.empty())
	{
		send(client_fd, "Invalid nickname\n", 17, 0);
	}
	//TODO: SEND MESSAGE TO THE INVITED USER
}

void IrcServer::topicCommand(int client_fd, std::string restOfCommand)
{
	if(_user->getOperator() == false)
	{
		send(client_fd, "Not allowed", 11, 0);
		return ;
	}
	std::string inputTopic = restOfCommand;
	if(inputTopic.empty())
	{
		//TODO: Show to op the topic of the channel
	}
	else
	{
		//TODO: Change topic of the channel
	}
}

void IrcServer::modeCommand(int client_fd, std::string restOfCommand)
{
	std::string mode = "itkol";
	restOfCommand = clean_input(restOfCommand, SPACES);
	if(_user->getOperator() == false)
	{
		send(client_fd, "Not allowed", 11, 0);
		return ;
	}
	if (!restOfCommand[0])
		send(client_fd, "Invalid mode\n", 13, 0);
	char inputMode = restOfCommand[0];
	std::string parameter = restOfCommand.substr(1);
	if (parameter.empty() && inputMode != 'i' && inputMode != 't')
		send(client_fd, "Invalid parameter\n", 18, 0);
	else
	{
		int i = 0;
		for(; i < 5; i++)
			if (inputMode == mode[i])
				break;
		switch (i)
		{
			case 0: //i (Set/remove Invite-only channel)
				_user->getChannel()->setInviteOnly(true);
				break;
			case 1: //t (Set/remove the restrictions of the TOPIC command to channel operators)
				//TODO:handle "t"
				break;
			case 2: //k (Set/remove the channel key)
				_user->getChannel()->setPassword(parameter);
				break;
			case 3: //o (Give/take channel operator privileges)
				getUserByNick(parameter)->setOperator(true);
				break;
			case 4: //l (Set the user limit to channel)
				_user->getChannel()->setLimit(std::atoi(parameter.c_str()));
				break;
			default:
				send(client_fd, "Invalid mode\n", 13, 0);
				break;
		}
	}
	//TODO: PRINT MESSAGES AFTER SETING MODE
	//TODO: Change mode of the channel
}

void IrcServer::parseCommand(int client_fd, std::string command)
{
	std::string commands[10] = {"/nick", "/join", "/leave", "/list", "/users", "/exit", "/kick",  "/invite", "/topic", "/mode"};
	int i = 0;
    std::string foundCommand;
    std::string restOfCommand;
	_user = getUserFd(client_fd);
    for(; i < 10; i++)
    {
        size_t pos = command.find(commands[i]);
        if(pos != std::string::npos)
        {
            foundCommand = commands[i];
            restOfCommand = command.substr(pos + commands[i].length());
			restOfCommand = clean_input(restOfCommand, ENTER);
            break;
        }
    }
	switch(i)
	{
		case 0:
			nickCommand(client_fd, restOfCommand);
			break;
		case 1:
			joinCommand(client_fd, restOfCommand);
			break;
		case 2:
			leaveCommand(client_fd);
			break;
		case 3:
			listCommand(client_fd, restOfCommand);
			break;
		case 4:
			usersCommand(client_fd);
			break;
		case 5:
			exitCommand(client_fd);
			break;
		case 6:
			kickCommand(client_fd, restOfCommand);
			break;
		case 7:
			inviteCommand(client_fd, restOfCommand);
			break;
		case 8:
			topicCommand(client_fd, restOfCommand);
			break;
		case 9:
			modeCommand(client_fd, restOfCommand);
			break;
		default:
			send(client_fd, "Invalid command\n", 16, 0);
			break;
	}
}

/*---------------------------- GETTERS/SETTERS---------------------------- */

/**
 * @brief Get the Socket object
 * 
 * @return int Socket
*/
int IrcServer::getSock() const
{
	return (_socket);
}

/**
 * @brief Get the Password object
 * 
 * @return std::string Password
*/
std::string IrcServer::getPwd() const
{
	return (_pwd);
}

/**
 * @brief Get the Poll Fds object
 * 
 * @return struct pollfd* Poll Fds
*/
struct pollfd &IrcServer::getPollFds(int i)
{
	return (_poll_fds[i]);
}

/**
 * @brief Get the Channels object
 * 
 * @return std::vector<Channel *> Channels
*/
std::vector<Channel *> IrcServer::getChannels() const
{
	return (_channels);
}

/**
 * @brief Get the Users object
 * 
 * @return std::vector<Client *> Users
*/
std::vector<Client *> IrcServer::getUsers() const
{
	return (_users);
}

/**
 * @brief Get the Channel By Name object
 * 
 * @param name Channel name
 * @return Channel* Channel
*/
Channel *IrcServer::getChannelByName(std::string name)
{
	for (size_t i = 0; i < _channels.size(); i++)
	{
		std::cout << "channel name[" << _channels[i]->getName() << "]" << std::endl;
		std::cout << "name[" << name << "]" << std::endl;
		if (_channels[i]->getName() == name)
		{
			return _channels[i];
		}
	}
	return NULL;
}

/**
 * @brief Get the User By Nick object
 * 
 * @param nick Nickname
 * @return Client* User
*/
Client *IrcServer::getUserByNick(std::string nick)
{
	for (size_t i = 0; i < _users.size(); i++)
	{
		if (_users[i]->getNick() == nick)
		{
			return _users[i];
		}
	}
	return NULL;
}

/**
 * @brief Get the User Fd object
 * 
 * @param fd File descriptor
 * @return std::vector<Client *> Users
*/
Client *IrcServer::getUserFd(int fd)
{
    for (size_t i = 0; i < _users.size(); i++)
    {
        if (_users[i]->getFd() == fd)
        {
            return _users[i];
        }
    }
    return NULL;
}

/**
 * @brief Set the Poll Fds object
 * 
 * @param i Index
 * @param fd File descriptor
 * @param revents Events
*/
void IrcServer::setPollFds(int i, int fd, short int revents)
{
	_poll_fds[i].fd = fd;
	_poll_fds[i].revents = revents;
}
