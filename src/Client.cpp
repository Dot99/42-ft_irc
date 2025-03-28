/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 10:26:01 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/28 09:47:53 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "IrcServer.hpp"

/**
 * @brief Construct a new Client:: Client object
 * @param server IrcServer object
*/
Client::Client(IrcServer &server) : _server(server)
{
	_isAuthenticated = false;
	_isOperator = false;
	_channel = NULL;
	_welcome_sent = false;
};

/**
 * @brief Destroy the Client:: Client object 
*/
Client::~Client()
{
}

/**
 * @brief Removes a client
 * 
 * @param client_fd Client file descriptor
*/
void Client::removeClient(int client_fd)
{
	for (size_t i = 0; i < 1; i++)
	{
		if (_server.getPollFds(i).fd == client_fd)
		{
			_server.getPollFds(i) = _server.getPollFds(1);
			break;
		}
	}
}

/**
 * @brief Handles a channel message
 * 
 * @param client_fd Client file descriptor
 * @param buffer Buffer
*/
void Client::handleChannelMessage(int client_fd, const std::string restOfCommand)
{
	std::string Name, message, msg;
	size_t msgPos, hashPos;
	int isCHannel = 0;

	hashPos = restOfCommand.find("#");
	if (hashPos != std::string::npos)
		isCHannel = 1;
	
	msgPos = restOfCommand.find(":");
	if (msgPos == std::string::npos || msgPos + 1 >= restOfCommand.size())
		return;
	if (isCHannel)
		Name = restOfCommand.substr(hashPos, msgPos - hashPos);
	else
		Name = restOfCommand.substr(0, msgPos);
	message = restOfCommand.substr(msgPos + 1);
    if (message.empty() || !_channel){
		return;
	}
	Name = clean_input(Name, SPACES);
	msg = ":" + getNick() + " PRIVMSG " + Name + " :" + message + "\r\n";
	if (isCHannel)
	{
		std::vector<Client *> users = _channel->getUsers();
		for (size_t i = 0; i < users.size(); i++)
		{
			if (users[i]->getFd() != client_fd)
				sendClientMsg(users[i]->getFd(), msg.c_str());
		}
	}
	else
	{
		Client *user = _server.getUserByNick(Name);
		if (user)
			sendClientMsg(user->getFd(), msg.c_str());
	}
}

/**
 * @brief Handles a client message
 * 
 * @param client_fd Client file descriptor
 * @param commands Commands object
*/
void Client::handleClientMessage(int client_fd)
{
	char buffer[1024];
	int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received < 0)
	{
		std::cout << "Client " << client_fd << " disconnected." << std::endl;
		close(client_fd);
		removeClient(client_fd);
		close(_server.getSock());
		return;
	}
	buffer[bytes_received] = '\0';
	_server.parseCommand(client_fd, buffer);
}

/**
 * @brief Checks if the password is correct
 * 
 * @param client_fd Client file descriptor
 * @return int Client file descriptor
*/
int Client::checkPwd(int client_fd)
{
	send(client_fd, "Enter server password: ", 23, 0);
	std::string input = readLine(client_fd, _server.getPwd().size());
	if(input.empty())
	{
		send(client_fd, "Invalid password\n", 17, 0);
		close(client_fd);
		return (-1);
	}
	if (input != _server.getPwd())
	{
		send(client_fd, "Invalid password", 16, 0);
		close(client_fd);
		return -1;
	}
	return (client_fd);
}

/**
 * @brief Accepts a client connection
 * @param client_fd Client file descriptor
*/
int Client::acceptClient(int client_fd)
{
	socklen_t client_addr_len = sizeof(_client_adrr);
	client_fd = accept(_server.getSock(), (struct sockaddr *)&_client_adrr, &client_addr_len);
	if(client_fd < 0)
	{
		std::cerr << "Error: Client connection failed" << std::endl;
		close(_server.getSock());
		return(-1);
	}
	setHost(std::string(inet_ntoa(_client_adrr.sin_addr)));
	setFd(client_fd);
	std::string input = readLine(client_fd, 512);
	if(input.empty())
	{
		close(client_fd);
		return (-1);
	}
	if(input.find("CAP LS") != std::string::npos)
		input = readLine(client_fd, 512);
	validateUser(client_fd, input);
	return(client_fd);
}

void Client::validateUser(int client_fd, std::string input)
{
	if(input.empty())
	{
		std::string command = "PASS";
		sendClientMsg(client_fd, ERR_NEEDMOREPARAMS(command));
		close(client_fd);
	}
	else
	{
		_server.parseCommand(client_fd, input);
		input = readLine(client_fd, 512);
		_server.parseCommand(client_fd, input);
		input = readLine(client_fd, 512);
		if(input.empty())
		{
			std::string command = "USER";
			sendClientMsg(client_fd, ERR_NEEDMOREPARAMS(command));
			close(client_fd);
		}
		else
			_server.parseCommand(client_fd, input);
	}
}



/*---------------------------- GETTERS/SETTERS---------------------------- */




/**
 * @brief Set the Nick object
 * 
 * @return std::string Nickname
*/
void Client::setUser(std::string user)
{
	_user = user;
}

/**
 * @brief Set the Nick object
 * 
 * @return std::string Nickname
*/
void Client::setNick(std::string nick)
{
	_nick = nick;
}

/**
 * @brief Get the User Nickname
 * 
 * @return std::string Nickname
*/
std::string Client::getNick()
{
	return _nick;
}

/**
 * @brief Get the User Username
 * 
 * @return std::string User
*/
std::string Client::getUser()
{
	return _user;
}

/**
 * @brief Set the Client File Descriptor
 * 
 * @param fd File Descriptor
*/
void Client::setFd(int fd)
{
	this->fd = fd;
}

/**
 * @brief Set the Channel object
 * 
 * @param channel Channel
*/
void Client::setChannel(Channel *channel)
{
	_channel = channel;
}

/**
 * @brief Get the Channel object
 * 
 * @return Channel* Channel
*/
Channel *Client::getChannel()
{
	return _channel;
}

/**
 * @brief Get the Client File Descriptor
 * 
 * @return int File Descriptor
*/
int Client::getFd()
{
	return fd;
}

/**
 * @brief Set the Authenticated object
 * 
 * @param auth If the client is authenticated
*/
void Client::setAuthenticated(bool auth)
{
	_isAuthenticated = auth;
}

/**
 * @brief Check if the client is authenticated
 * 
 * @return true If the client is authenticated
 * @return false If the client is not authenticated
*/
bool Client::getAuthenticated()
{
	return (_isAuthenticated);
}

/**
 * @brief Set the Operator object
 * 
 * @param op If the client is an operator
*/
void Client::setOperator(bool op)
{
	_isOperator = op;
}

/**
 * @brief Set the Welcome Sent object
 * 
 * @param welcome_sent If the welcome message was sent
*/
void Client::setWelcomeSent(bool welcome_sent)
{
	_welcome_sent = welcome_sent;
}

/**
 * @brief Get the Welcome Sent object
 * 
 * @return true If the welcome message was sent
 * @return false If the welcome message was not sent
*/
bool Client::getWelcomeSent()
{
	return (_welcome_sent);
}

/**
 * @brief Check if the client is an operator
 * 
 * @return true If the client is an operator
 * @return false If the client is not an operator
*/
bool Client::getOperator()
{
	return (_isOperator);
}

/**
 * @brief Set the Host object
 * 
 * @param host Host
*/
void Client::setHost(std::string host)
{
	_host = host;
}

/**
 * @brief Get the Host object
 * 
 * @return std::string Host
*/
std::string Client::getHost()
{
	return _host;
}

/**
 * @brief Set the Real Name object
 * 
 * @param real_name Real Name
*/
void Client::setRealName(std::string real_name)
{
	_real_name = real_name;
}

/**
 * @brief Get the Real Name object
 * 
 * @return std::string Real Name
*/
std::string Client::getRealName()
{
	return _real_name;
}