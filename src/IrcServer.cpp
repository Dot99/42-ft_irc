/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 11:14:16 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/20 12:32:11 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcServer.hpp"

/**
 * @brief Construct a new IrcServer object
 * 
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
	setArgs(args);
	startServer();
}

/**
 * @brief Destroy the IrcServer object
 * 
 */
IrcServer::~IrcServer()
{
	close(_socket);
	for (size_t i = 0; i < _users.size(); i++)
	{
		delete _users[i];
	}
	for (size_t i = 0; i < _channels.size(); i++)
	{
		delete _channels[i];
	}
	_poll_fds.clear();
}

/**
 * @brief Start the server
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
		throw std::exception();
	}
	int opt = 1;
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
	{
		std::cerr << "Error: Setsockopt failed" << std::endl;
		close(_socket);
		throw std::exception();
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
	}
	if (listen(_socket, 5) < 0)
	{
		std::cerr << "Error: Listen failed" << std::endl;
		close(_socket);
		throw std::exception();
	}
	std::cout << "Server started on port: " << _port << std::endl;
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
						throw std::exception();
					}
					struct pollfd newPoll;
					newPoll.fd = client_fd;
					newPoll.events = POLLIN;
					newPoll.revents = 0;
					_poll_fds.push_back(newPoll);
					//_users.back()->validateUser(client_fd);
					std::string server = SERVER_NAME;
					if (!_users.back()->getNick().empty() && !_users.back()->getUser().empty() && _users.back()->getAuthenticated())
						sendClientMsg(client_fd, RPL_WELCOME(_users.back()->getNick(), server));
				}
				else
				{
					getUserFd(_poll_fds[i].fd)->handleClientMessage(_poll_fds[i].fd);
				}
			}	
		}
	}
}

/**
 * @brief Set the arguments
 * 
 * @param args Arguments
 */
void IrcServer::setArgs(const std::string args[])
{
	std::istringstream iss(args[1]);
	if (!(iss >> _port))
	{
		std::cerr << "Erro: Invalid Port\n";
		throw std::exception();
	}
	setPwd(args[2]);
	// Get current time
	std::time_t now = std::time(0);
	std::tm *ltm = std::localtime(&now);
	std::stringstream ss;
	ss << 1900 + ltm->tm_year << "-"
		<< 1 + ltm->tm_mon << "-"
		<< ltm->tm_mday << " "
		<< ltm->tm_hour << ":"
		<< ltm->tm_min << ":"
		<< ltm->tm_sec;
	_now = ss.str();
	std::cout << RPL_YOURHOST(SERVER_NAME) << std::endl;
	std::cout << RPL_CREATED(_now) << std::endl;
	std::cout << RPL_MYINFO(SERVER_NAME, "1.0") << std::endl;
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


/*---------------------------- COMMANDS ---------------------------- */

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
						send(client_fd, msg.c_str(), msg.length(), 0);
						break;
					}
					else if (_channels[i]->getPassword() != passwd)
					{
						send(client_fd, "Channel has password\n", 21, 0);
						break;
					}
					_channels[i]->addUser(_user);
					_user->setChannel(_channels[i]);
					send(client_fd, msg.c_str(), msg.length(), 0);
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
		_user->setOperator(true);
		if (channel->getTopic().empty())
			channel->setTopic("General");
	}
	if (_user->getChannel())
	{	
		std::string msg = ":" + _user->getNick() + " JOIN " + _user->getChannel()->getName() + "\r\n";
		sendClientMsg(client_fd, msg);
		sendClientMsg(client_fd, RPL_TOPIC(_user->getNick(), _user->getChannel()->getName(), _user->getChannel()->getTopic()));
		std::string user_list;
		std::vector<Client *> users = _user->getChannel()->getUsers();
		for (size_t i = 0; i < users.size(); ++i)
		{
			user_list += users[i]->getNick();
			if (i + 1 != users.size())
				user_list += " ";
		}
		for (size_t i = 0; i < users.size(); i++)
			sendClientMsg(users[i]->getFd(), RPL_NAMREPLY(_user->getNick(), _user->getChannel()->getName(), user_list));
		sendClientMsg(client_fd, RPL_ENDOFNAMES(_user->getNick(), _user->getChannel()->getName()));
	}
	//TODO:Check if channel exists and if it has pwd
}

/**
 * @brief Command to leave a channel
 * @param client_fd File descriptor of the client
*/
void IrcServer::partCommand(int client_fd, std::string restOfCommand)
{
	std::istringstream iss(restOfCommand);
	std::string channelName;
	std::string reason;

	iss >> channelName >> reason;
	if(channelName.empty() && _user->getChannel() == NULL)
	{
		std::string msg = "PART";
		sendClientMsg(client_fd, ERR_NEEDMOREPARAMS(msg));
		return ;
	}
	if(channelName.empty() && _user->getChannel() != NULL)
	{
		_user->getChannel()->removeUser(_user);
		_user->setChannel(NULL);
	}
	if (!getChannelByName(channelName))
	{
		sendClientMsg(client_fd, ERR_NOSUCHCHANNEL(channelName));
		return ;
	}
	if (_user->getChannel()->getName() != channelName)
	{
		sendClientMsg(client_fd, ERR_NOTONCHANNEL(_user->getNick(), channelName));
		return ;
	}
	std::vector<Client *> users = _user->getChannel()->getUsers();
	for (size_t i = 0; i < users.size(); i++)
		sendClientMsg(users[i]->getFd(), _user->getNick() + " is leaving the channel" + _user->getChannel()->getName() + "\n");
	sendClientMsg(_user->getFd(), ":" + _user->getNick() + "!" + _user->getUser() + "@" + _user->getHost() + " PART " + _user->getChannel()->getName() + "\n");
	_user->getChannel()->removeUser(_user);
	_user->setChannel(NULL);
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
		for (size_t i = 0; i < _channels.size(); i++)
		{
			if (wildcardMatch(_channels[i]->getName(), restOfCommand))
			{
				send(client_fd, "Channel: ", 9, 0);
				send(client_fd, _channels[i]->getName().c_str(), _channels[i]->getName().length(), 0);
				send(client_fd, "\n", 1, 0);
			}
		}
	}
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
	if(_user->getOperator() == false)
	{
		size_t pos = restOfCommand.find("#");
		if(pos == std::string::npos)
		{
			sendClientMsg(client_fd, ERR_CHANNOPRIVSNEEDED(restOfCommand));
			return ;
		}
		else
		{
			std::string channelName = restOfCommand.substr(pos);
			if(channelName.find(' ') != std::string::npos)
			{
				while(channelName.find(' ') != std::string::npos)
					channelName = channelName.substr(channelName.find(' ') + 1);
				sendClientMsg(client_fd, ERR_UNKNOWNMODE(channelName));
			}
			else
			{				
				sendClientMsg(client_fd, ERR_NOSUCHCHANNEL(channelName));
				return ;
			}
		}
	}
	else
	{
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
}

void IrcServer::passCommand(int client_fd, std::string restOfCommand)
{
	if (_user->getAuthenticated() == true)
	{
		// sendClientMsg(client_fd, ERR_ALREADYREGISTRED);
	}
	else if(restOfCommand != _pwd)
	{
		sendClientMsg(client_fd, ERR_PASSWDMISMATCH);
		//TODO: Close connection
	}
	else
	{
		_user->setAuthenticated(true);
	}
}

/**
 * @brief Command to change the nickname of the user
 * @param client_fd File descriptor of the client
*/
int IrcServer::nickCommand(int client_fd, std::string restOfCommand)
{
	std::string msg = checkNick(restOfCommand, _users);
	if (!msg.empty())
	{
		sendClientMsg(client_fd, msg);
		return (1);
	}
	else
	{
		_user->setNick(restOfCommand);
		return (0);
	}
	return (0);
}

void IrcServer::userCommand(int client_fd, std::string restOfCommand)
{
	std::istringstream iss(restOfCommand);
	std::string user;
	std::string zero;
	std::string asterisk;
	std::string realName;
	
	iss >> user >> zero >> asterisk >> realName;
	std::string inputUser = restOfCommand;
	if(inputUser.empty())
	{
		std::string user = "USER";
		sendClientMsg(client_fd, ERR_NEEDMOREPARAMS(user));	
	}
	else
	{
		_user->setUser(user);
		_user->setRealName(realName);
		if (!_user->getNick().empty())
			sendClientMsg(client_fd, "CAP * LS");
	}
}

void IrcServer::quitCommand(int client_fd, std::string restOfCommand)
{
	//TODO: Send msg to channel/s saying user left with QUIT
	sendClientMsg(client_fd, "ERROR :Closing Link: (Quit: " + restOfCommand + ")");
	close(client_fd);
}

void IrcServer::whoCommand(int client_fd, std::string restOfCommand)
{
	std::string msg;
	if (restOfCommand.empty())
	{
		for (size_t i = 0; i < _users.size(); i++)
		{
			msg = RPL_WHOREPLY(_users[i]->getNick(), _users[i]->getChannel()->getName(), _users[i]->getUser(), _users[i]->getHost(), _users[i]->getRealName());
			sendClientMsg(client_fd, msg);
		}
		msg = RPL_ENDOFWHO(restOfCommand);
		sendClientMsg(client_fd, msg);
		return ;
	}
	size_t pos = restOfCommand.find("#");
	if (pos != std::string::npos)
	{
		for (size_t i = 0; i < _users.size(); i++)
		{
			if(_users[i]->getNick() == restOfCommand)
			{
				msg = RPL_WHOREPLY(_users[i]->getNick(), _users[i]->getChannel()->getName(), _users[i]->getUser(), _users[i]->getHost(), _users[i]->getRealName());
				break;
			}
		}
		msg = RPL_ENDOFWHO(restOfCommand);
		return ;
	}
	else
	{
		if(getChannelByName(restOfCommand))
		{
			//LIST ALL USERS ON CHANNEL
			std::vector<Client *> users = getChannelByName(restOfCommand)->getUsers();
			for (size_t i = 0; i < users.size(); i++)
			{
				msg = RPL_WHOREPLY(users[i]->getNick(), users[i]->getChannel()->getName(), users[i]->getUser(), users[i]->getHost(), users[i]->getRealName());
				sendClientMsg(client_fd, msg);
			}
			msg = RPL_ENDOFWHO(restOfCommand);
			sendClientMsg(client_fd, msg);
		}
		else
		{
			msg = RPL_ENDOFWHO(restOfCommand);
			return ;
		}
	}
}

void IrcServer::parseAuthenticate(int client_fd, std::string parameter)
{
	std::string parameters[3] = {"PASS", "NICK", "USER"};
	int i = 0;
	std::string foundParameter;
	std::string restOfParameter;
	_user = getUserFd(client_fd);
	for(; i < 3; i++)
	{
		size_t pos = parameter.find(parameters[i]);
		if(pos != std::string::npos)
		{
			foundParameter = parameters[i];
			restOfParameter = parameter.substr(pos + parameters[i].length());
			restOfParameter = clean_input(restOfParameter, SPACES);
			break;
		}
	}
	switch(i)
	{

			break;
		// default:
		// 	sendClientMsg(client_fd, ERR_UNKNOWNCOMMAND);
		// 	break;
	}
}

void IrcServer::parseCommand(int client_fd, std::string command)
{
	std::string commands[14] = {"JOIN", "PART", "LIST", "EXIT", "KICK",  "INVITE", "TOPIC", "MODE","PASS", "NICK", "USER", "PRIVMSG", "QUIT", "WHO"};
	int i = 0;
	std::string foundCommand;
	std::string restOfCommand;
	_user = getUserFd(client_fd);
	for(; i < 14; i++)
	{
		size_t pos = command.find(commands[i]);
		if(pos != std::string::npos)
		{
			foundCommand = commands[i];
			restOfCommand = command.substr(pos + commands[i].length());
			if (foundCommand != "INVITE" &&  foundCommand != "USER" && foundCommand != "PART")
				restOfCommand = clean_input(restOfCommand, SPACES);
			else
				restOfCommand = clean_input(restOfCommand, ENTER);
			break;
		}
	}
	switch(i)
	{
		case 0:
			joinCommand(client_fd, restOfCommand);
			break;
		case 1:
			partCommand(client_fd, restOfCommand);
			break;
		case 2:
			listCommand(client_fd, restOfCommand);
			break;
		case 3:
			exitCommand(client_fd);
			break;
		case 4:
			kickCommand(client_fd, restOfCommand);
			break;
		case 5:
			inviteCommand(client_fd, restOfCommand);
			break;
		case 6:
			topicCommand(client_fd, restOfCommand);
			break;
		case 7:
			modeCommand(client_fd, restOfCommand);
			break;
		case 8:
			passCommand(client_fd, restOfCommand);
			break;
		case 9:
			nickCommand(client_fd, restOfCommand);
			break;
		case 10:
			userCommand(client_fd, restOfCommand);
			break;
		case 11:
			_user->handleChannelMessage(client_fd, restOfCommand);
			break;
		case 12:
			quitCommand(client_fd, restOfCommand);
			break;
		case 13:
			whoCommand(client_fd, restOfCommand);
			break;
		default:
			std::cout << "Unknown command" << std::endl;
			//TODO:: Unknown command
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

/**
 * @brief Set the Password object
 * 
 * @param pwd Password
*/
void IrcServer::setPwd(std::string pwd)
{
	_pwd = pwd;
}