/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 11:14:16 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/26 10:04:33 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcServer.hpp"

/**
 * @brief Construct a new IrcServer object
 * 
 * @param args Arguments
 */
IrcServer::IrcServer(const std::vector<std::string> &args)
{
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
	_users.clear();
	for (size_t i = 0; i < _channels.size(); i++)
	{
		delete _channels[i];
	}
	_channels.clear();
	std::vector<pollfd>().swap(_poll_fds);
	_now.clear();
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
					if (!_users.back()->getNick().empty() && !_users.back()->getUser().empty() && _users.back()->getAuthenticated())
					{
						sendClientMsg(client_fd, RPL_WELCOME(_users.back()->getNick(), SERVER_NAME));
						_users.back()->setWelcomeSent(true);
					}
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
void IrcServer::setArgs(const std::vector<std::string> &args)
{
	std::istringstream iss(args[1]);
	if (!(iss >> _port))
	{
		std::cerr << "Error: Invalid Port\n";
		throw std::exception();
	}
	if(_port < 0 || _port > 65535)
	{
		std::cerr << "Error: Invalid Port\n";
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
	std::string msg;
	if(channelName.empty())
	{
		send(client_fd, "Invalid channel\n", 16, 0);
		return ;
	}
	bool channelExists = false;
	Channel *channel = NULL;
	// Check if channel exists
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (_channels[i]->getName() == channelName)
		{
			channelExists = true;
			channel = _channels[i];
			break;
		}
	}
	if (channelExists && _user->getChannel() == NULL)
    {
        // Check invite-only and password conditions
        if (channel->getInviteOnly() && !_user->getOperator() && !channel->getInvitedUser(client_fd))
        {
            send(client_fd, "Channel is invite only\n", 24, 0);
            return;
        }
        if (channel->getLimit() > 0 && channel->getUsers().size() >= channel->getLimit())
        {
            send(client_fd, "Channel is full\n", 16, 0);
            return;
        }
        if (!channel->getPassword().empty() && channel->getPassword() != passwd)
        {
            send(client_fd, "Channel has password\n", 21, 0);
            return;
        }

        // Add user to channel
        channel->addUser(_user);
        _user->setChannel(channel);
    }
    else if (!channelExists) // Create new channel
    {
        channel = new Channel(channelName);
        addChannel(channel);
        channel->addUser(_user);
        _user->setChannel(channel);
        _user->setOperator(true);
        if (channel->getTopic().empty())
            channel->setTopic(" ");
    }
	if (_user->getChannel())
	{
		std::vector<Client *> users = _user->getChannel()->getUsers();
		for (size_t i = 0; i < users.size(); ++i)
			sendClientMsg(users[i]->getFd(), RPL_JOIN(_user->getNick(), _user->getChannel()->getName()));
		
		std::string user_list;
		for (size_t i = 0; i < users.size(); ++i)
		{
			if(users[i]->getOperator())
				user_list += "@";
			user_list += users[i]->getNick();
			if (i + 1 != users.size())
				user_list += " ";
		}
        sendClientMsg(client_fd, RPL_NAMREPLY(_user->getNick(), _user->getChannel()->getName(), user_list));
        sendClientMsg(client_fd, RPL_ENDOFNAMES(_user->getNick(), _user->getChannel()->getName()));
		if(_user->getChannel()->getTopic() != " ")
			sendClientMsg(client_fd, RPL_TOPIC(_user->getNick(), _user->getChannel()->getName(), _user->getChannel()->getTopic()));
	}
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
	std::string msg = ":" + _user->getNick() + "!" + _user->getUser() + "@" + _user->getHost() + " PART " + _user->getChannel()->getName();
	if(!restOfCommand.empty())
		msg += " :" + restOfCommand + "\r\n";
	else
		msg += "\r\n";
	sendClientMsg(_user->getFd(), msg);
	_user->getChannel()->removeUser(_user);
	_user->setChannel(NULL);
	if(getChannelByName(channelName)->getUsers().empty())
		delete getChannelByName(channelName);
}

/**
 * @brief Command to list all channels
 * @param client_fd File descriptor of the client
*/
void IrcServer::listCommand(int client_fd, std::string restOfCommand)
{
	sendClientMsg(client_fd, RPL_LIST);
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
	sendClientMsg(client_fd, RPL_LISTEND);
}

void IrcServer::exitCommand(int client_fd)
{
	_user->getChannel()->removeUser(_user);
	_user->setChannel(NULL);
	close(client_fd);
}

void IrcServer::kickCommand(int client_fd, std::string restOfCommand)
{
	std::istringstream iss(restOfCommand);
	std::string channelName;
	std::string inputNick;
	std::string message;
	std::string msg;
	iss >> channelName >> inputNick >> message;
	if(_user->getOperator() == false)
	{
		send(client_fd, "Not allowed", 11, 0);
		return ;
	}
	if(inputNick.empty())
	{
		send(client_fd, "Invalid nickname\n", 17, 0);
	}
	if(_user->getChannel()->getName() == channelName)
	{
		for (size_t i = 0; i < _user->getChannel()->getUsers().size(); i++)
		{
			if (_user->getChannel()->getUsers()[i]->getNick() == inputNick)
			{
				_user->getChannel()->getUsers()[i]->setOperator(false);
				_user->getChannel()->getUsers()[i]->setChannel(NULL);
				_user->getChannel()->removeInvitedUser(_user->getChannel()->getUsers()[i]);
				if (message.empty())
					msg = ":" + _user->getNick() + " KICK " + channelName + " " + inputNick + "\r\n";
				else
					msg = ":" + _user->getNick() + " KICK " + channelName + " " + inputNick + " :" + message + "\r\n";
				for (size_t j = 0; j < _user->getChannel()->getUsers().size(); j++)
				{
					sendClientMsg(_user->getChannel()->getUsers()[j]->getFd(), msg);
				}
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
	if(_user->getOperator() == false || _user->getChannel() == NULL)
	{
		if(_user->getChannel()->getTopicProtection())
		{
			sendClientMsg(client_fd, ERR_CHANNOPRIVSNEEDED(_user->getChannel()->getName()));
			return ;
		}
	}
	std::string inputTopic = restOfCommand;
	if(inputTopic.empty())
	{
		if(_user->getChannel()->getTopic().empty())
			sendClientMsg(client_fd, RPL_NOTOPIC(_user->getChannel()->getName()));
		else
		{
			sendClientMsg(client_fd, RPL_TOPIC(_user->getNick(), _user->getChannel()->getName(), _user->getChannel()->getTopic()));
			sendClientMsg(client_fd, RPL_TOPICWHOTIME(_user->getChannel()->getName(), _user->getNick(), _user->getChannel()->getTopicTime()));
		}
	}
	else
	{
		_user->getChannel()->setTopic(inputTopic);
		std::time_t now = std::time(0);
		std::tm *ltm = std::localtime(&now);
		std::stringstream ss;
		ss << 1900 + ltm->tm_year << "-"
			<< 1 + ltm->tm_mon << "-"
			<< ltm->tm_mday << " "
			<< ltm->tm_hour << ":"
			<< ltm->tm_min << ":"
			<< ltm->tm_sec;
		std::string tnow = ss.str();
		_user->getChannel()->setTopicTime(tnow);
		std::vector<Client *> users = _user->getChannel()->getUsers();
		for (size_t i = 0; i < users.size(); i++)
		{
			std::string msg = ":" + _user->getNick() + "!" + _user->getUser() + "@" + _user->getHost() + " TOPIC " + _user->getChannel()->getName() + " :" + inputTopic + "\r\n";
			sendClientMsg(users[i]->getFd(), msg);
		}
	}
}

void IrcServer::modeCommand(int client_fd, std::string restOfCommand)
{
	std::string mode = "itkol";
	std::istringstream iss(restOfCommand);
	std::string target;
	std::string inputMode;
	std::string parameter;
	std::string msg = "MODE";
	iss >> target >> inputMode >> parameter;
	if(_user->getOperator() == false)
	{
		size_t pos = restOfCommand.find("#");
		if(pos == std::string::npos || !inputMode.empty())
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
	else // If user operator
	{
		restOfCommand = clean_input(restOfCommand, ENTER);
		if(restOfCommand.empty())
		{
			std::string msg = "MODE";
			sendClientMsg(client_fd, ERR_NEEDMOREPARAMS(msg));
			return ;
		}
		else
		{
			if (target != _user->getChannel()->getName())
			{
				if(getChannelByName(_user->getChannel()->getName()))
				{
					sendClientMsg(client_fd, ERR_CHANNOPRIVSNEEDED(_user->getChannel()->getName()));
					return ;
				}
				else
				{
					sendClientMsg(client_fd, ERR_NOSUCHCHANNEL(_user->getChannel()->getName()));
					return ;
				}
			}
			else
			{
				if(inputMode.empty() && !target.empty())
				{
					sendClientMsg(client_fd, RPL_CHANNELMODEIS(_user->getChannel()->getName(), "+-", mode));
					return ;
				}
				if (inputMode.empty())
				{
					sendClientMsg(client_fd, ERR_NEEDMOREPARAMS(msg));
					return ;
				}
				else
				{
					if (inputMode[0] != '+' && inputMode[0] != '-')
					{
						std::string signal = "+-";
						sendClientMsg(client_fd, RPL_CHANNELMODEIS(_user->getChannel()->getName(), signal, mode));
						return ;
					}
					else
					{
						if (inputMode[1] != 'i' && inputMode[1] != 't' && inputMode[1] != 'k' && inputMode[1] != 'o' && inputMode[1] != 'l')
						{
							sendClientMsg(client_fd, ERR_UNKNOWNMODE(inputMode));
							return ;
						}
						else
						{
							int i = 0;
							for(; i < 5; i++)
								if (inputMode[1] == mode[i])
									break;
							switch (i)
							{
								case 0: //i (Set/remove Invite-only channel)
									if(inputMode[0] == '+')
									{
										_user->getChannel()->setInviteOnly(true);
										std::string msg = ":" + _user->getNick() + " MODE " + _user->getChannel()->getName() + " +i \r\n";
										sendClientMsg(client_fd, msg);
									}
									else
									{
										_user->getChannel()->setInviteOnly(false);
										std::string msg = ":" + _user->getNick() + " MODE " + _user->getChannel()->getName() + " -i \r\n";
										sendClientMsg(client_fd, msg);
									}
									break;
								case 1: //t (Set/remove the restrictions of the TOPIC command to channel operators)
									if(inputMode[0] == '+')
									{
										_user->getChannel()->setTopicProtection(true);
										std::string msg = ":" + _user->getNick() + " MODE " + _user->getChannel()->getName() + " +t \r\n";
										sendClientMsg(client_fd, msg);
									}
									else
									{
										_user->getChannel()->setTopicProtection(false);
										std::string msg = ":" + _user->getNick() + " MODE " + _user->getChannel()->getName() + " +t \r\n";
										sendClientMsg(client_fd, msg);
									}
									break;
								case 2: //k (Set/remove the channel key)
									if (parameter.empty() && inputMode[0] == '+')
									{
										sendClientMsg(client_fd, ERR_NEEDMOREPARAMS(msg));
										return ;
									}
									else
										if(inputMode[0] == '+')
										{
											_user->getChannel()->setPassword(parameter);
											std::string msg = ":" + _user->getNick() + " MODE " + _user->getChannel()->getName() + " +k " + parameter + "\r\n";
											sendClientMsg(client_fd, msg);
										}
										else
										{
											_user->getChannel()->setPassword("");
											std::string msg = ":" + _user->getNick() + " MODE " + _user->getChannel()->getName() + " +k " + parameter + "\r\n";
											sendClientMsg(client_fd, msg);
										}
									break;
								case 3: //o (Set/remove a user as channel operator)
									if (parameter.empty())
									{
										sendClientMsg(client_fd, ERR_NEEDMOREPARAMS(msg));
										return ;
									}
									else
									{
										if(getUserByNick(parameter) == NULL)
										{
											sendClientMsg(client_fd, ERR_NOSUCHNICK(parameter));
											return ;
										}
										else
										{
											if(inputMode[0] == '+')
											{
												getUserByNick(parameter)->setOperator(true);
												std::string msg = ":" + _user->getNick() + " MODE " + _user->getChannel()->getName() + " +o " + parameter + "\r\n";
												for(size_t i = 0; i < _user->getChannel()->getUsers().size(); i++)
													sendClientMsg(_user->getChannel()->getUsers()[i]->getFd(), msg);
											}
											else
											{
												getUserByNick(parameter)->setOperator(false);
												std::string msg = ":" + _user->getNick() + " MODE " + _user->getChannel()->getName() + " -o " + parameter + "\r\n";
												for(size_t i = 0; i < _user->getChannel()->getUsers().size(); i++)
													sendClientMsg(_user->getChannel()->getUsers()[i]->getFd(), msg);
											}
										}
									}
									break;
								case 4: //l (Set/remove the user limit to channel)
									if (parameter.empty() && inputMode[0] == '+')
									{
										sendClientMsg(client_fd, ERR_NEEDMOREPARAMS(msg));
										return ;
									}
									else
									{
										if(inputMode[0] == '+')
										{
											_user->getChannel()->setLimit(std::atoi(parameter.c_str()));
											std::string msg = ":" + _user->getNick() + " MODE " + _user->getChannel()->getName() + " +l " + parameter + "\r\n";
											sendClientMsg(client_fd, msg);
										}
										else
											_user->getChannel()->setLimit(0);
									}
									break;
								default:
									break;
							}
						}
					}
				}
			}
		}
		//TODO: PRINT MESSAGES AFTER SETING MODE
	}
}

void IrcServer::passCommand(int client_fd, std::string restOfCommand)
{
	//TODO: VERIFICAR A UTILIDADE DE getAuthenticated
	if (_user->getAuthenticated() == true)
	{
		// sendClientMsg(client_fd, ERR_ALREADYREGISTRED);
	}
	else if(restOfCommand != _pwd)
	{
		sendClientMsg(client_fd, ERR_PASSWDMISMATCH);
		close(client_fd);
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
void IrcServer::nickCommand(int client_fd, std::string restOfCommand)
{
	_user = getUserFd(client_fd);
    if (!_user) 
    {
        std::cout << "Error: User not found" << std::endl;
        return ;
    }

	std::string inputNick = clean_input(restOfCommand, ENTER);
	std::string msg = checkNick(inputNick, _users);
	if (!msg.empty())
	{
		sendClientMsg(client_fd, msg);
		return ;
	}
	_user->setNick(inputNick);
	if(_user->getChannel())
	{
		std::vector<Client *> users = _user->getChannel()->getUsers();
		for (size_t i = 0; i < users.size(); i++)
		{
			std::string msg = ":" + _user->getNick() + "!" + _user->getUser() + "@" + _user->getHost() + " NICK " + inputNick + "\r\n";
			sendClientMsg(users[i]->getFd(), msg);
		}
	}
	if (!_user->getUser().empty())
		sendClientMsg(client_fd, "CAP * LS\r\n");
	if (!_user->getWelcomeSent() && !_user->getUser().empty() && !_user->getNick().empty())
		sendClientMsg(client_fd, RPL_WELCOME(_user->getNick(), SERVER_NAME));
	return ;
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
	if(user.empty())
	{
		std::string user = "USER";
		sendClientMsg(client_fd, ERR_NEEDMOREPARAMS(user));
		return ;
	}
	else if (user.length() < USERLEN)
		_user->setUser(user);
	else
		_user->setUser(user.substr(0, USERLEN));
	_user->setRealName(realName);
	if (!_user->getNick().empty())
		sendClientMsg(client_fd, "CAP * LS\r\n");
}

void IrcServer::quitCommand(int client_fd, std::string restOfCommand)
{
	//TODO: Send msg to channel/s saying user left with QUIT
	if(_user->getChannel())
	{
		std::string channelName = _user->getChannel()->getName();
		std::vector<Client *> users = _user->getChannel()->getUsers();
		for (size_t i = 0; i < users.size(); i++)
			sendClientMsg(users[i]->getFd(), _user->getNick() + " is leaving the channel" + _user->getChannel()->getName() + "\n");
		std::string msg = ":" + _user->getNick() + "!" + _user->getUser() + "@" + _user->getHost() + " PART " + _user->getChannel()->getName();
		if(!restOfCommand.empty())
			msg += " :" + restOfCommand + "\r\n";
		else
			msg += "\r\n";
		sendClientMsg(_user->getFd(), msg);
		_user->getChannel()->removeUser(_user);
		_user->setChannel(NULL);
		if(getChannelByName(channelName)->getUsers().empty())
			delete getChannelByName(channelName);
	}
	if(restOfCommand.empty())
		sendClientMsg(client_fd, "ERROR :Closing Link: (Quit: )");
	else
		sendClientMsg(client_fd, "ERROR :Closing Link: (Quit: " + restOfCommand + ")");
	close(client_fd);
}

void IrcServer::whoCommand(int client_fd, std::string restOfCommand)
{
	std::istringstream iss(restOfCommand);
	std::string channel;
	iss >> channel;

	if(channel.empty() || channel[0] != '#')
	{
		sendClientMsg(client_fd, ERR_NOSUCHCHANNEL(channel));
		return ;
	}
	if(!getChannelByName(channel))
	{
		sendClientMsg(client_fd, ERR_NOSUCHCHANNEL(channel));
		return;
	}
	for(size_t i = 0; i < getChannelByName(channel)->getUsers().size(); i++)
	{
		std::string flag = " H";
		if (getChannelByName(channel)->getUsers()[i]->getOperator())
			flag += " @";
		std::string msg = RPL_WHOREPLY(getChannelByName(channel)->getUsers()[i]->getNick(), getChannelByName(channel)->getUsers()[i]->getHost(), 
		getChannelByName(channel)->getUsers()[i]->getChannel()->getName(), 
		getChannelByName(channel)->getUsers()[i]->getUser(), getChannelByName(channel)->getUsers()[i]->getRealName(), flag);
		sendClientMsg(client_fd, msg);
	}
	sendClientMsg(client_fd, RPL_ENDOFWHO(_user->getNick(), channel));
}

void IrcServer::parseCommand(int client_fd, std::string command)
{
	std::string commands[14] = {"JOIN", "PART", "LIST", "EXIT", "KICK",  "INVITE", "TOPIC", "MODE", "PASS", "NICK", "USER", "PRIVMSG", "QUIT", "WHO"};
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
			if (foundCommand != "INVITE" &&  foundCommand != "USER" && foundCommand != "PART" && foundCommand != "MODE" && foundCommand != "PRIVMSG"
				&& foundCommand != "TOPIC" && foundCommand != "WHO" && foundCommand != "KICK")
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
			sendClientMsg(client_fd, ERR_UNKNOWNCOMMAND(restOfCommand));
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