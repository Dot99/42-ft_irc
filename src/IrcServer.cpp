/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 11:14:16 by gude-jes          #+#    #+#             */
/*   Updated: 2025/04/03 10:32:33 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcServer.hpp"

/**
 * @brief Construct a new IrcServer object
 *
 * @param args Arguments
 */
IrcServer::IrcServer(char **argv, int argc)
{
	_args.reserve(argc);
	for (int i = 0; i < argc; i++)
		_args.push_back(std::string(argv[i]));
	setArgs(_args);
	startServer();
}

/**
 * @brief Copy assignment operator
 *
 * @param rhs IrcServer object to copy
 * @return IrcServer& Reference to the current object
 */
IrcServer &IrcServer::operator=(const IrcServer &rhs)
{
	if (this != &rhs)
	{
		_port = rhs._port;
		_socket = rhs._socket;
		_pwd = rhs._pwd;
		_now = rhs._now;
		_server_addr = rhs._server_addr;
		_poll_fds = rhs._poll_fds;
		_channels = rhs._channels;
		_users = rhs._users;
		_args = rhs._args;
		_user = rhs._user;
	}
	return *this;
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
		if (_users[i])
		{
			close(_users[i]->getFd());
			delete _users[i];
		}
	}
	_users.clear();
	std::vector<Client *>().swap(_users);
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (_channels[i])
		{
			delete _channels[i];
			_channels[i] = NULL;
		}
	}
	_channels.clear();
	_args.clear();
	std::vector<std::string>().swap(_args);
	for (size_t i = 0; i < _poll_fds.size(); i++)
	{
		if (_poll_fds[i].fd != -1)
			close(_poll_fds[i].fd);
	}
	_poll_fds.clear();
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
	if (_socket < 0)
		throw std::runtime_error("Error: Socket creation failed");
	int opt = 1;
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
	{
		throw std::runtime_error("Error: Setsockopt failed");
		close(_socket);
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
		close(_socket);
		throw std::runtime_error("Error: Bind failed");
	}
	if (listen(_socket, 5) < 0)
	{
		close(_socket);
		throw std::runtime_error("Error: Listen failed");
	}
	std::cout << "Server started on port: " << _port << std::endl;
}

/**
 * @brief Run the server
 *
 */
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
					addUser(newClient);
					client_fd = _users.back()->acceptClient(_poll_fds[i].fd);
					if (client_fd < 0)
					{
						std::cerr << "Error accepting client" << std::endl;
						removeUser(newClient);
						continue;
					}
					struct pollfd newPoll;
					newPoll.fd = client_fd;
					newPoll.events = POLLIN;
					newPoll.revents = 0;
					_poll_fds.push_back(newPoll);
					if (!getUserFd(client_fd)->getNick().empty() && !getUserFd(client_fd)->getUser().empty() && getUserFd(client_fd)->getAuthenticated() && !getUserFd(client_fd)->getWelcomeSent())
					{
						sendClientMsg(client_fd, "CAP * LS\r\n");
						sendClientMsg(client_fd, RPL_WELCOME(getUserFd(client_fd)->getNick(), SERVER_NAME));
						sendClientMsg(client_fd, RPL_YOURHOST(SERVER_NAME));
						sendClientMsg(client_fd, RPL_CREATED(_now));
						sendClientMsg(client_fd, RPL_MYINFO(SERVER_NAME, getUserFd(client_fd)->getNick(), "1.0"));
						sendClientMsg(client_fd, RPL_MOTDSTART(getUserFd(client_fd)->getNick()));
						sendMotd(client_fd, getUserFd(client_fd)->getNick());
						sendClientMsg(client_fd, RPL_ENDOFMOTD(getUserFd(client_fd)->getNick()));
						getUserFd(client_fd)->setWelcomeSent(true);
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
		throw std::runtime_error("Error: Invalid Port");
	if (_port < 0 || _port > 65535)
		throw std::runtime_error("Error: Invalid Port");
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
}

/*---------------------------- COMMANDS ---------------------------- */

/**
 * @brief Command to join a channel
 * @param client_fd File descriptor of the client
 */
void IrcServer::joinCommand(int client_fd, std::string restOfCommand)
{
	if(restOfCommand.empty())
		return;
	std::istringstream iss(restOfCommand);
	std::string channelName;
	std::string passwd;
	bool channelExists = false;
	Channel *channel = NULL;

	iss >> channelName >> passwd;
	if (channelName.empty())
	{
		sendClientMsg(client_fd, ERR_NOSUCHCHANNEL(channelName));
		return;
	}
	std::transform(channelName.begin(), channelName.end(), channelName.begin(), ::tolower);
	if(channelName[0] && channelName[0] != '#')
	{
		sendClientMsg(client_fd, ERR_BADCHANMASK(channelName));
		return;
	}
	for (size_t i = 0; i < _user->getChannels().size(); i++)
	{
		if (_user->getChannels()[i]->getName() == channelName)
		{
			sendClientMsg(client_fd, ERR_USERONCHANNEL(_user->getNick(), channelName));
			return;
		}
	}
	channel = checkChannelName(channelName, _channels);
	if (channel)
		channelExists = true;
	if (channelExists)
	{
		// Check invite-only and password conditions
		if (channel->getInviteOnly() && !_user->getOperator() && !channel->getInvitedUser(client_fd))
		{
			sendClientMsg(client_fd, ERR_INVITEONLYCHAN(channelName));
			return;
		}
		if (channel->getLimit() > 0 && channel->getUsers().size() >= channel->getLimit())
		{
			sendClientMsg(client_fd, ERR_CHANNELISFULL(channelName));
			return;
		}
		if (!channel->getPassword().empty() && channel->getPassword() != passwd)
		{
			sendClientMsg(client_fd, ERR_PASSWDMISMATCH);
			return;
		}
		
		// Add user to channel
		channel->addUser(_user);
		_user->addChannel(channel);
	}
	else if (!channelExists) // Create new channel
	{
		channel = new Channel(channelName);
		addChannel(channel);
		channel->addUser(_user);
		_user->addChannel(channel);
		_user->setOperator(true);
		channel->setTopic("general");
	}
	std::vector<Client *> users = channel->getUsers();
	std::string msg;
	for (size_t i = 0; i < users.size(); ++i)
	{
		msg = RPL_JOIN(_user->getNick(), _user->getUser(), _user->getHost(), channel->getName());
		sendClientMsg(users[i]->getFd(), msg);
	}
	std::string user_list;
	for (size_t i = 0; i < users.size(); ++i)
	{
		if (users[i]->getOperator())
			user_list += "@";
		user_list += users[i]->getNick();
		if (i + 1 != users.size())
			user_list += " ";
	}
	sendClientMsg(client_fd, RPL_TOPIC2(_user->getNick(), channel->getName(), channel->getTopic()));
	sendClientMsg(client_fd, RPL_NAMREPLY(_user->getNick(), channel->getName(), user_list));
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
	if (channelName.empty())
	{
		std::string msg = "PART";
		sendClientMsg(client_fd, ERR_NEEDMOREPARAMS(msg));
		return;
	}
	Channel *channel = getChannelByName(channelName);
	if(!channel)
	{
		sendClientMsg(client_fd, ERR_NOSUCHCHANNEL(channelName));
		return;
	}
	if (std::find(channel->getUsers().begin(), channel->getUsers().end(), _user) == channel->getUsers().end())
	{
		sendClientMsg(client_fd, ERR_NOTONCHANNEL(_user->getNick(), channelName));
		return;
	}
	std::string msg = ":" + _user->getNick() + "!" + _user->getUser() + "@" + _user->getHost() + " PART " + channel->getName();
	if (!restOfCommand.empty())
		msg += " :" + restOfCommand + "\r\n";
	else
		msg += "\r\n";
	std::vector<Client *> users = channel->getUsers();
	for (size_t i = 0; i < users.size(); i++)
		sendClientMsg(users[i]->getFd(), msg);
	channel->removeUser(_user);
	_user->removeChannel(channel);
	if (channel->getUsers().empty())
	{
		_channels.erase(std::remove(_channels.begin(), _channels.end(), channel), _channels.end());
		delete channel;
	}
}

/**
 * @brief Command to list all channels
 * @param client_fd File descriptor of the client
 */
void IrcServer::listCommand(int client_fd, std::string restOfCommand)
{
	sendClientMsg(client_fd, RPL_LISTSTART);
	std::string msg;
	std::string clientCount;
	if (restOfCommand.empty())
	{
		for (size_t i = 0; i < _channels.size(); i++)
		{
			clientCount = to_string(_channels[i]->getUsers().size());
			msg = RPL_LIST(_channels[i]->getName(), clientCount, _channels[i]->getTopic());
			sendClientMsg(client_fd, msg);
		}
	}
	else
	{
		for (size_t i = 0; i < _channels.size(); i++)
		{
			if (wildcardMatch(_channels[i]->getName(), restOfCommand))
			{
				clientCount = to_string(_channels[i]->getUsers().size());
				msg = RPL_LIST(_channels[i]->getName(), clientCount, _channels[i]->getTopic());
				sendClientMsg(client_fd, msg);
			}
		}
	}
	sendClientMsg(client_fd, RPL_LISTEND);
}

/**
 * @brief Command to exit the server
 * @param client_fd File descriptor of the client
 */
void IrcServer::exitCommand(int client_fd)
{
	//Loop through all channels and remove user from them
	for(size_t i = 0; i < _channels.size(); i++)
	{
		for(size_t j = 0; j < _channels[i]->getUsers().size(); j++)
		{
			if (_channels[i]->getUsers()[j]->getFd() == client_fd)
			{
				_channels[i]->removeUser(_user);
				_user->removeChannel(_channels[i]);
				close(client_fd);
				break;
			}
		}
	}
}

/**
 * @brief Command to kick a user from a channel
 * @param client_fd File descriptor of the client
 */
void IrcServer::kickCommand(int client_fd, std::string restOfCommand)
{
	std::istringstream iss(restOfCommand);
	std::string channelName;
	std::string inputNick;
	std::string message;
	std::string msg;
	iss >> channelName >> inputNick >> message;
	if (_user->getOperator() == false)
	{
		sendClientMsg(client_fd, ERR_CHANOPRIVSNEEDED(channelName));
		return;
	}
	if (inputNick.empty())
	{
		sendClientMsg(client_fd, ERR_NONICKNAMEGIVEN);
		return;
	}
	
	Channel *channel = getChannelByName(channelName);
	if(channel)
	{
		sendClientMsg(client_fd, ERR_NONICKNAMEGIVEN);
		return;
	}
	else
	{
		Channel *channel = getChannelByName(channelName);
		if (!channel)
		{
			sendClientMsg(client_fd, ERR_NOTONCHANNEL(_user->getNick(), channelName));
			return;
		}
		Client *userToKick = NULL;
		for (size_t i = 0; i < channel->getUsers().size(); i++)
		{
			if (channel->getUsers()[i]->getNick() == inputNick)
			{
				userToKick = channel->getUsers()[i];
				break;
			}
		}
		if (!userToKick)
		{
			sendClientMsg(client_fd, ERR_NOSUCHNICK(inputNick));
			return;
		}
		if (message.empty())
			msg = ":" + _user->getNick() + " KICK " + channelName + " " + inputNick + "\r\n";
		else
			msg = ":" + _user->getNick() + " KICK " + channelName + " " + inputNick + " :" + message + "\r\n";
		for (size_t j = 0; j < channel->getUsers().size(); j++)
		{
			sendClientMsg(channel->getUsers()[j]->getFd(), msg);
		}
		userToKick->setOperator(false);
		userToKick->removeChannel(channel);
		channel->removeInvitedUser(userToKick);
		channel->removeUser(userToKick);
	}
}

/**
 * @brief Command to invite a user to a channel
 * @param client_fd File descriptor of the client
 */
void IrcServer::inviteCommand(int client_fd, std::string restOfCommand)
{
	std::istringstream iss(restOfCommand);
	std::string nickname;
	std::string channelName;

	iss >> channelName >> nickname;
	if (nickname.empty())
	{
		sendClientMsg(client_fd, ERR_NOSUCHNICK(nickname));
		return;
	}
	Channel *channel = getChannelByName(channelName);
	if (!channel)
	{
		sendClientMsg(client_fd, ERR_NOTONCHANNEL(_user->getNick(), channelName));
		return;
	}
	if (_user->getOperator() == false && channel->getInviteOnly() == true)
	{
		sendClientMsg(client_fd, ERR_CHANOPRIVSNEEDED(channel->getName()));
		return;
	}
	if (!getChannelByName(channelName))
	{
		sendClientMsg(client_fd, ERR_NOSUCHCHANNEL(channelName));
		return;
	}
	if (!getUserByNick(nickname))
	{
		sendClientMsg(client_fd, ERR_NOSUCHNICK(nickname));
		return;
	}
	for(size_t i = 0; i < _channels.size(); i++)
	{
		if (_channels[i]->getName() == channelName)
		{
			sendClientMsg(client_fd, ERR_USERONCHANNEL(nickname, channelName));
			return;
		}
	}
	channel->addInvitedUser(getUserByNick(nickname));
	sendClientMsg(client_fd, RPL_INVITING(_user->getNick(), channelName));
	std::string inputNick = restOfCommand;
	if (inputNick.empty())
	{
		sendClientMsg(client_fd, ERR_NOSUCHNICK(nickname));
	}
	sendClientMsg(getUserByNick(nickname)->getFd(), "INVITE " + _user->getNick() + " " + channelName + "\r\n");
}

/**
 * @brief Command to set the topic of a channel
 * @param client_fd File descriptor of the client
 */
void IrcServer::topicCommand(int client_fd, std::string restOfCommand)
{
	std::istringstream iss(restOfCommand);
	std::string channelName;
	std::string inputTopic;
	iss >> channelName >> inputTopic;
	if (inputTopic[0] && inputTopic[0] == ':')
		inputTopic = inputTopic.substr(1);
	Channel *channel = getChannelByName(channelName);
	if (_user->getOperator() == false || !channel)
	{
		if (!channel->getTopicProtection() && channel)
		{
			sendClientMsg(client_fd, ERR_CHANOPRIVSNEEDED(channel->getName()));
			return;
		}
		else if (!channel)
		{
			sendClientMsg(client_fd, ERR_NOSUCHCHANNEL(channelName));
			return;
		}
	}
	if (inputTopic.empty())
	{
		if (!channel->getTopic().empty())
			sendClientMsg(client_fd, RPL_NOTOPIC(channel->getName()));
		else
		{
			sendClientMsg(client_fd, RPL_TOPIC(channel->getName(), channel->getTopic()));
			sendClientMsg(client_fd, RPL_TOPICWHOTIME(channel->getName(), _user->getNick(), channel->getTopicTime()));
		}
	}
	else
	{
		channel->setTopic(inputTopic);
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
		channel->setTopicTime(tnow);
		std::vector<Client *> users = channel->getUsers();
		for (size_t i = 0; i < users.size(); i++)
		{
			std::string msg = ":" + _user->getNick() + "!" + _user->getUser() + "@" + _user->getHost() + " TOPIC " + channel->getName() + " :" + inputTopic + "\r\n";
			sendClientMsg(users[i]->getFd(), msg);
		}
	}
}

/**
 * @brief Command to set the mode of a channel
 * @param client_fd File descriptor of the client
 */
void IrcServer::modeCommand(int client_fd, std::string restOfCommand)
{
	std::string modes = "itkol";
	std::istringstream iss(restOfCommand);
	std::vector<std::string> arguments;
	std::string argument;
	std::string msg = "MODE";
	while(iss >> argument)
		arguments.push_back(argument);
	if(arguments.empty())
	{
		sendClientMsg(client_fd, ERR_NEEDMOREPARAMS(msg));
		arguments.clear();
		return;
	}
	std::string channelName = arguments[0];
	if(channelName.empty())
	{
		sendClientMsg(client_fd, ERR_NOSUCHCHANNEL(channelName));
		return;
	}
	Channel *channel = getChannelByName(channelName);
	channel = checkChannelName(channelName, _channels);
	if(!channel)
	{
		sendClientMsg(client_fd, ERR_NOSUCHCHANNEL(channelName));
		return;
	}
	if(!_user->getOperator())
	{
		if (channelName[0] && channelName[0] != '#')
		{
			sendClientMsg(client_fd, ERR_BADCHANMASK(channelName));
			return;
		}
		else
		{
			sendClientMsg(client_fd, RPL_CHANNELMODEIS(channel->getName(), "+-", modes));
			return ;
		}
	}
	if(channelName[0] && channelName[0] != '#')
	{
		sendClientMsg(client_fd, ERR_BADCHANMASK(channelName));
		return;
	}
	if(arguments.size() < 2 || arguments[1].empty())
	{
		sendClientMsg(client_fd, RPL_CHANNELMODEIS(channel->getName(), "+-", modes));
		arguments.clear();
		return;
	}

	bool positive = false;
	bool flag = false;
	size_t i = 0;
	if(arguments[1][0] == '+' || arguments[1][0] != '-')
	{
		positive = true;
		if (arguments[1][0] == '+')
			i++;
		else
			flag = true;
	}
	else if (arguments[1][0] == '-')
		i++;
	for(; i < arguments[1].length(); i++)
	{
		if(arguments[1][i] != 'i' && arguments[1][i] != 't' && arguments[1][i] != 'k'
			&& arguments[1][i] != 'o' && arguments[1][i] != 'l' && arguments[1][i] != '+' && arguments[1][i] != '-')
		{
			sendClientMsg(client_fd, ERR_UNKNOWNMODE(arguments[1]));
			return;
		}
		switch (arguments[1][i])
		{
			case 'i': // i (Set/remove Invite-only channel)
				if(positive)
				{
					channel->setInviteOnly(true);
					msg = ":" + _user->getNick() + " MODE " + channel->getName() + " +i \r\n";
					sendClientMsg(client_fd, msg);
				}
				else
				{
					channel->setInviteOnly(false);
					msg = ":" + _user->getNick() + " MODE " + channel->getName() + " -i \r\n";
					sendClientMsg(client_fd, msg);
					positive = true;
				}
				break;
			case 't': // t (Set/remove the restrictions of the TOPIC command to channel operators)
				if(positive)
				{
					channel->setTopicProtection(true);
					msg = ":" + _user->getNick() + " MODE " + channel->getName() + " +t \r\n";
					sendClientMsg(client_fd, msg);
				}
				else
				{
					channel->setTopicProtection(false);
					msg = ":" + _user->getNick() + " MODE " + channel->getName() + " -t \r\n";
					sendClientMsg(client_fd, msg);
					positive = true;
				}
				break;
			case 'k': // k (Set/remove the channel key)
				if(positive)
				{
					if (i + 1 < arguments.size())
					{
						channel->setPassword(arguments[i + 1]);
						if(!flag)
							msg = ":" + _user->getNick() + " MODE " + channel->getName() + " +k " + arguments[i + 1] + "\r\n";
						else
							msg = ":" + _user->getNick() + " MODE " + channel->getName() + " +k " + arguments[i + 2] + "\r\n";
						sendClientMsg(client_fd, msg);
					}
					else
					{
						sendClientMsg(client_fd, ERR_NEEDMOREPARAMS(msg));
					}
				}
				else
				{
					channel->setPassword("");
					msg = ":" + _user->getNick() + " MODE " + channel->getName() + " -k \r\n";
					sendClientMsg(client_fd, msg);
					positive = true;
				}
				break;
			case 'o': // o (Set/remove channel operator)
				if(positive)
				{
					if (i + 1 < arguments.size())
                	{
						Client *targetUser = getUserByNick(arguments[i + 1]);
						if (targetUser)
						{
							targetUser->setOperator(true);
							if(!flag)
								msg = ":" + _user->getNick() + " MODE " + channel->getName() + " +o " + arguments[i + 1] + "\r\n";
							else
								msg = ":" + _user->getNick() + " MODE " + channel->getName() + " +o " + arguments[i + 2] + "\r\n";
							sendClientMsg(client_fd, msg);
						}
						else
						{
							sendClientMsg(client_fd, ERR_NOSUCHNICK(arguments[i + 1]));
						}
					}
					else
						sendClientMsg(client_fd, ERR_NEEDMOREPARAMS(msg));
				}
				else
				{
					if (i + 1 < arguments.size())
					{
						Client *targetUser = getUserByNick(arguments[i + 1]);
						if (targetUser)
						{
							targetUser->setOperator(false);
							if(!flag)
								msg = ":" + _user->getNick() + " MODE " + channel->getName() + " -o " + arguments[i + 1] + "\r\n";
							else
								msg = ":" + _user->getNick() + " MODE " + channel->getName() + " -o " + arguments[i + 2] + "\r\n";
							sendClientMsg(client_fd, msg);
						}
						else
						{
							sendClientMsg(client_fd, ERR_NOSUCHNICK(arguments[i + 1]));
						}
					}
					else
					{
						sendClientMsg(client_fd, ERR_NEEDMOREPARAMS(msg));
					}
				}
				break;
			case 'l': // l (Set/remove channel limit)
				if(positive)
				{
					if (i + 1 < arguments.size())
					{
						channel->setLimit(atoi(arguments[i + 1].c_str()));
						if(!flag)
							msg = ":" + _user->getNick() + " MODE " + channel->getName() + " +l " + arguments[i + 1] + "\r\n";
						else
							msg = ":" + _user->getNick() + " MODE " + channel->getName() + " +l " + arguments[i + 2] + "\r\n";
						sendClientMsg(client_fd, msg);
					}
					else
					{
						sendClientMsg(client_fd, ERR_NEEDMOREPARAMS(msg));
					}
				}
				else
				{
					channel->setLimit(0);
					if(!flag)
						msg = ":" + _user->getNick() + " MODE " + channel->getName() + " -l " + "\r\n";
					else
						msg = ":" + _user->getNick() + " MODE " + channel->getName() + " -l " + "\r\n";
					sendClientMsg(client_fd, msg);
					positive = true;
				}
				break;
			default:
				break;
		}
	}
	arguments.clear();	
}

/**
 * @brief Command to set the password of the server
 * @param client_fd File descriptor of the client
 */
void IrcServer::passCommand(int client_fd, std::string restOfCommand)
{
	if (restOfCommand != _pwd)
	{
		sendClientMsg(client_fd, ERR_PASSWDMISMATCH);
		sendClientMsg(client_fd, "ERROR :Closing Link: Pass: Password incorrect\r\n");
		close(client_fd);
	}
	_user->setPasswordVerified(true);
}

/**
 * @brief Command to change the nickname of the user
 * @param client_fd File descriptor of the client
 */
void IrcServer::nickCommand(int client_fd, std::string restOfCommand)
{
	//_user = getUserFd(client_fd);
	if (!_user)
	{
		std::cout << "Error: User not found" << std::endl;
		return;
	}
	std::string inputNick = clean_input(restOfCommand, ENTER);
	std::string msg = checkNick(inputNick, _users);
	if (!msg.empty())
		sendClientMsg(client_fd, msg);
	if (!_user->getNick().empty())
	{
		msg = ":" + _user->getNick() + " NICK " + inputNick + "\r\n";
		sendClientMsg(client_fd, msg);
	}
	if (_user->getPasswordVerified())
		_user->setNick(inputNick);
	else
	{
		sendClientMsg(client_fd, "INTRODUCE PASSWORD FIRST -> PASS <password>\r\n");
		return;
	}
	for (size_t j = 0; j < _channels.size(); j++)
	{
			std::vector<Client *> users = _channels[j]->getUsers();
			for (size_t i = 0; i < users.size(); i++)
			{
				msg = ":" + _user->getNick() + "!" + _user->getUser() + "@" + _user->getHost() + " NICK " + inputNick + "\r\n";
				sendClientMsg(users[i]->getFd(), msg);
			}
	}
	return;
}

/**
 * @brief Command to set the user of the client
 * @param client_fd File descriptor of the client
 */
void IrcServer::userCommand(int client_fd, std::string restOfCommand)
{
	std::istringstream iss(restOfCommand);
	std::string user;
	std::string zero;
	std::string asterisk;
	std::string realName;

	iss >> user >> zero >> asterisk >> realName;
	std::string inputUser = restOfCommand;
	if (user.empty())
	{
		std::string user = "USER";
		sendClientMsg(client_fd, ERR_NEEDMOREPARAMS(user));
		return;
	}
	if (_user->getAuthenticated())
	{
		sendClientMsg(client_fd, ERR_ALREADYREGISTERED);
		return;
	}
	if (!_user->getPasswordVerified())
	{
		sendClientMsg(client_fd, "INTRODUCE PASSWORD FIRST -> PASS <password>\r\n");
		return;
	}
	if (user.length() < USERLEN)
		_user->setUser(user);
	else
		_user->setUser(user.substr(0, USERLEN));
	if (zero.empty() || zero != "0")
	{
		std::string zero = "USER";
		sendClientMsg(client_fd, ERR_NEEDMOREPARAMS(zero));
		return;
	}
	if (asterisk.empty() || asterisk != "*")
	{
		std::string asterisk = "USER";
		sendClientMsg(client_fd, ERR_NEEDMOREPARAMS(asterisk));
		return;
	}
	if (realName.empty())
	{
		std::string realName = "USER";
		sendClientMsg(client_fd, ERR_NEEDMOREPARAMS(realName));
		return;
	}
	if (realName[0] == ':')
		realName = realName.substr(1);
	_user->setRealName(realName);
	if (_user->getNick() != "" && !_user->getAuthenticated())
		_user->setAuthenticated(true);
}

/**
 * @brief Command to quit the server
 * @param client_fd File descriptor of the client
 */
void IrcServer::quitCommand(int client_fd, std::string restOfCommand)
{
	std::vector <Channel *> channels = _user->getChannels();
	if(channels.size() != 0)
	{
		for (size_t i = 0; i < channels.size(); i++)
		{
			Channel *channel = getChannelByName(channels[i]->getName());
			if(!channel)
				continue;
			for (size_t j = 0; j < channels[i]->getUsers().size(); j++)
			{
				std::vector<Client *> users = channels[i]->getUsers();
				std::string msg = ":" + getUserFd(client_fd)->getNick() + "!" + getUserFd(client_fd)->getUser() + "@" + getUserFd(client_fd)->getHost() + " PART " + channel->getName();
				for(size_t z = 0; z < users.size(); z++)
				{
					if(users[z] && users[z]->getFd() != client_fd)
						sendClientMsg(users[i]->getFd(), getUserFd(client_fd)->getNick() + " is leaving the channel" + channel->getName() + "\n");
				}
				if(!restOfCommand.empty())
					msg += " :" + restOfCommand + "\r\n";
				else
					msg += "\r\n";
				sendClientMsg(_user->getFd(), msg);
			}
			channels[i]->removeUser(getUserFd(client_fd));
			if (channels[i]->getUsers().empty())
				removeChannelByName(channels[i]->getName());
			getUserFd(client_fd)->removeChannel(channels[i]);
		}
	}
	if (restOfCommand.empty())
		sendClientMsg(client_fd, "ERROR :Closing Link: Quit: Client Quit");
	else
		sendClientMsg(client_fd, "ERROR :Closing Link: Quit: " + restOfCommand);
	for (size_t i = 0; i < _users.size(); i++)
	{
		if (_users[i]->getFd() == client_fd)
		{
			delete _users[i];
			_users[i] = NULL;
			_users.erase(_users.begin() + i);
			break;
		}
	}
	for (size_t i = 0; i < _poll_fds.size(); i++)
	{
		if (_poll_fds[i].fd == client_fd)
		{
			_poll_fds.erase(_poll_fds.begin() + i);
			break;
		}
	}
	close(client_fd);
}

/**
 * @brief Command to get the list of users in a channel
 * @param client_fd File descriptor of the client
 */
void IrcServer::whoCommand(int client_fd, std::string restOfCommand)
{
	std::istringstream iss(restOfCommand);
	std::string channel;
	iss >> channel;

	if (channel.empty() || channel[0] != '#')
	{
		sendClientMsg(client_fd, ERR_NOSUCHCHANNEL(channel));
		return;
	}
	if (!getChannelByName(channel))
	{
		sendClientMsg(client_fd, ERR_NOSUCHCHANNEL(channel));
		return;
	}
	for (size_t i = 0; i < getChannelByName(channel)->getUsers().size(); i++)
	{
		std::string flag = "H";
		if (getChannelByName(channel)->getUsers()[i]->getOperator())
			flag += " @";
		std::string msg = RPL_WHOREPLY(
			SERVER_NAME,
			getChannelByName(channel)->getUsers()[i]->getNick(),	// client (the one issuing WHO)
			getChannelByName(channel)->getName(),					// channel
			getChannelByName(channel)->getUsers()[i]->getUser(),	// username
			getChannelByName(channel)->getUsers()[i]->getHost(),	// host
			getChannelByName(channel)->getUsers()[i]->getNick(),	// nick
			flag,													// flags
			getChannelByName(channel)->getUsers()[i]->getRealName() // realname
		);
		sendClientMsg(client_fd, msg);
	}
	sendClientMsg(client_fd, RPL_ENDOFWHO(_user->getNick(), channel));
}

/**
 * @brief Command to parse the command received from the client
 * @param client_fd File descriptor of the client
 * @param command Command received from the client
 */
void IrcServer::parseCommand(int client_fd, std::string command)
{
	if (command.empty() || command == "\r\n")
		return;
	std::string commands[14] = {"JOIN", "PART", "LIST", "EXIT", "KICK", "INVITE", "TOPIC", "MODE", "PASS", "NICK", "USER", "PRIVMSG", "QUIT", "WHO"};
	int i = 0;
	std::string foundCommand;
	std::string restOfCommand;
	_user = getUserFd(client_fd);
	for (; i < 14; i++)
	{
		size_t pos = command.find(commands[i]);
		if (pos != std::string::npos)
		{
			foundCommand = commands[i];
			restOfCommand = command.substr(pos + commands[i].length());
			if (foundCommand != "INVITE" && foundCommand != "USER" && foundCommand != "PART" && foundCommand != "MODE" && foundCommand != "PRIVMSG" && foundCommand != "TOPIC" && foundCommand != "WHO" && foundCommand != "KICK" && foundCommand != "JOIN")
				restOfCommand = clean_input(restOfCommand, SPACES);
			else
				restOfCommand = clean_input(restOfCommand, ENTER);
			break;
		}
	}
	switch (i)
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
	if (fd < 0 || _users.size() == 0)
		return NULL;
	for (size_t i = 0; i < _users.size(); i++)
	{
		if (_users[i]->getFd() == fd)
			return _users[i];
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
void IrcServer::setPwd(std::string const &pwd)
{
	_pwd = pwd;
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
 * @brief Set the Client object
 *
 * @param client Client
 */
void IrcServer::removeUser(Client *client)
{
	for (size_t i = 0; i < _users.size(); i++)
	{
		if (_users[i] == client)
		{
			delete _users[i];
			_users.erase(_users.begin() + i);
			break;
		}
	}
}

/**
 * @brief Find a channel by name
 *
 * @param name Channel name
 * @param channels List of channels
 * @return Channel* Channel
 */
void IrcServer::removeChannelByName(const std::string &name)
{
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (_channels[i]->getName() == name)
		{
			delete _channels[i];
			_channels.erase(_channels.begin() + i);
		}
	}
}