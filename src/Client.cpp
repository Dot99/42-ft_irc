/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 10:26:01 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/13 17:14:05 by gude-jes         ###   ########.fr       */
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
	// _client.erase(client_fd);
}


void Client::validateUser(int client_fd)
{
	std::string inputNick;
	while (true)
	{
		inputNick = readLine(client_fd, 9); //(9) Max Nickname characters
		if (inputNick.empty())
		{
			send(client_fd, "Invalid nickname\n", 17, 0);
			continue; // Ask for nickname again
		}
		if (inputNick == getNick())
		{
			break; // Valid nickname
		}
		else
		{
			if(inputNick == getNick())
			{
				send(client_fd, "Nickname already in use\n", 24, 0);
			}
			else
			{
				setUser(inputNick, "");
				break;
			}
		}
	}

	send(client_fd, "PASS:\n", 5, 0);
	std::string inputPwd = readLine(client_fd, 510); //(510) Max pwd characters
	if (inputPwd.empty())
	{
		send(client_fd, "Invalid password\n", 17, 0);
		close(client_fd);
		return;
	}
	std::cout << inputPwd << std::endl;
	std::cout << getPass() << std::endl;
	if (inputPwd == getPass())
	{
		setUser(inputNick, inputPwd);
		setAuthenticated(true);
		send(client_fd, "Welcome to IRC server\n\n", 23, 0);
	}
	//TODO: Check if getPass() is empty due to \n
	else if (getPass().empty())
	{
		setUser(inputNick, inputPwd);
		setAuthenticated(true);
		send(client_fd, "Welcome to IRC server\n\n", 23, 0);
	}
	else
	{
		send(client_fd, "Invalid password\n", 17, 0);
		close(client_fd);
	}
}

/**
 * @brief Handles a channel message
 * 
 * @param client_fd Client file descriptor
 * @param buffer Buffer
*/
void Client::handleChannelMessage(int client_fd, char *buffer)
{
	if (buffer[0] == '\n')
		return;
	std::string msg = getNick() + ": " + buffer;
	if (_channel)
	{
		for (size_t i = 0; i < _channel->getUsers().size(); i++)
		{
			if (_channel->getUsers()[i]->getFd() != client_fd)
				sendClientMsg(_channel->getUsers()[i]->getFd(), msg.c_str(), 0);
		}
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
	if (bytes_received < 0) // Client disconnected
	{
		std::cout << "Client " << client_fd << " disconnected." << std::endl;
		close(client_fd);
		removeClient(client_fd);
		close(_server.getSock());
		return;
	}
	buffer[bytes_received] = '\0';
	if (buffer[0] && buffer[0] == '/')
		_server.parseCommand(client_fd, buffer);
	else
	{
		handleChannelMessage(client_fd, buffer);
	}
	// TODO: Parse and process IRC commands here
	/*
		Check if there is a command sent by client
		if command is NICK
			Check if nickname is already in use
			If not
				Set nickname
				Send welcome message
			Else
				Send error message
		Else if command is JOIN
			Check if channel exists
			If not
				Create channel
			Join channel
	*/
	
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
		// close(_server.getSock());
		return(-1);
	}
	std::cout << "Client connected" << std::endl;
	return (checkPwd(client_fd));
}





/*---------------------------- GETTERS/SETTERS---------------------------- */




/**
 * @brief Set the Nick object
 * 
 * @return std::string Nickname
*/
void Client::setUser(std::string nick, std::string pass)
{
	_nick = nick;
	_pass = pass;
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
 * @brief Get the Nick object
 * 
 * @return std::string Nickname
*/
std::string Client::getPass()
{
	return _pass;
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
 * @brief Check if the client is an operator
 * 
 * @return true If the client is an operator
 * @return false If the client is not an operator
*/
bool Client::getOperator()
{
	return (_isOperator);
}