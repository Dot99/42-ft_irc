/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 10:26:01 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/13 08:55:39 by gude-jes         ###   ########.fr       */
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

/**
 * @brief Adds a user to the map
 * 
 * @param nick Nickname
 * @param pass Password
*/
void Client::addUserData(std::string nick, std::string pass)
{
	if (!_user[nick].empty())
	{
		_user[nick] = pass;
	}
}


void Client::validateUser(int client_fd)
{
	std::string inputNick = readLine(client_fd, 9); //(9) Max Nickname characters
	if(inputNick.empty())
	{
		//TODO: Send user to nick again due to error
		send(client_fd, "Invalid nickname\n", 17, 0);
		close(client_fd);
		//return (-1);
	}
	if (inputNick == getNick())
	{
		send(client_fd, "PASS:\n", 5, 0);
		std::string inputPwd = readLine(client_fd, 510); //(510) Max pwd characters
		if(inputPwd.empty())
		{
			//TODO: Send user to nick again due to error
			send(client_fd, "Invalid password\n", 17, 0);
			close(client_fd);
			//return (-1);
		}
		if(inputPwd == getPass())
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
	else
	{
		setUser(inputNick, "");
		send(client_fd, "PASS:\n", 5, 0);
		std::string inputPwd = readLine(client_fd, 510); //(510) Max pwd characters
		if(inputPwd.empty())
		{
			//TODO: Send user to nick again due to error
			send(client_fd, "Invalid password\n", 17, 0);
			close(client_fd);
			//return (-1);
		}
		setUser(inputNick, inputPwd);
		// setAuthenticated(true);
		std::string listOfComands = "\n\nList of comands:\n/nick /join /leave /list /users /exit /kick /invite /topic /mode\n\n";
		send(client_fd, listOfComands.c_str(), listOfComands.length(), 0);
	}
	if (_server.getChannels()[1])
	{
		std::cout << "users in channels: " << _server.getChannels()[1]->getUsers().size() << std::endl;
		if (_server.getChannels()[1]->getUsers()[0])
		{
			std::cout << "users in channels: " << _server.getChannels()[1]->getUsers()[0]->getNick() << std::endl;
		}
	}
}

/**
 * @brief Handles a client message
 * 
 * @param client_fd Client file descriptor
 * @param commands Commands object
*/
void Client::handleClientMessage(int client_fd, Commands &commands)
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
    commands.parseCommand(client_fd, buffer);
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