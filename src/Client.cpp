/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 10:26:01 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/10 13:08:11 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "IrcServer.hpp"

Client::Client(IrcServer &server) : _server(server)
{
};

Client::~Client()
{
}

/**
 * @brief Set the Nick object
 * 
 * @param nick Nickname
*/
void Client::setNick(std::string nick)
{
	_nick = nick;
}

/**
 * @brief Set the User object
 * 
 * @param user Username
*/
void Client::setUser(std::string user)
{
	_user = user;
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
 * @brief Get the Nick object
 * 
 * @return std::string Nickname
*/
std::string Client::getNick()
{
	return (_nick);
}

/**
 * @brief Get the User object
 * 
 * @return std::string Username
*/
std::string Client::getUser()
{
	return (_user);
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
 * @brief Handles a client message
 * 
 * @param client_fd Client file descriptor
 * @param commands Commands object
*/
void Client::handleClientMessage(int client_fd, Commands &commands)
{
    char buffer[1024];
	if (_isAuthenticated == false)
	{
		commands.parseCommand(buffer, "", client_fd, buffer);
		return;
	}
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
    std::cout << buffer << std::endl;
    // TODO: Parse and process IRC commands here
	/*
		Check if there is a command sent by client
		Check if client already has a nickname
		If not
			Check if command is NICK
			Check if nickname is already in use
			If not
				Set nickname
				Send welcome message
			Else
				Send error message
		Else
			Check if command is JOIN
			Check if channel exists
			If not
				Create channel
			Join channel
	*/
	
}


/**
 * @brief Accepts a client connection
 * 
*/
int Client::acceptClient(int client_fd)
{
	std::cout << "poste nas virilhas" << std::endl;
	socklen_t client_addr_len = sizeof(_client_adrr);
	client_fd = accept(_server.getSock(), (struct sockaddr *)&_client_adrr, &client_addr_len);
	// std::cout << "a minha avo tem tetano" << std::endl;
	if(client_fd < 0)
	{
		std::cerr << "Error: Client connection failed" << std::endl;
		close(_server.getSock());
		return -1;
		//exit(1);
	}
	std::cout << "Client connected" << std::endl;
	//Ask for password
	char buffer[1024];
	send(client_fd, "Enter password: ", 16, 0);
	int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received <= 0) // Client disconnected
	{
		std::cout << "Client " << client_fd << " disconnected." << std::endl;
		close(client_fd);
		// close(_server.getSock());
		return -1;
	}
	buffer[bytes_received] = '\0';
	//Check password
	std::string input(buffer);
	std::string cleaned_input = cleanInput(input);
	if (cleaned_input != _server.getPwd())
	{
		send(client_fd, "Invalid password", 16, 0);
		close(client_fd);
		return -1;
	}
	send(client_fd, "PASS:\n", 6, 0);
	return (client_fd);
}