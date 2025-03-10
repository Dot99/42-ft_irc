/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 10:26:01 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/10 12:57:13 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "IrcServer.hpp"

Client::Client(IrcServer* server) : _server(server), _clients()
{
};

Client::Client(IrcServer* server) : _server(server)
{
}

Client::~Client()
{
}


/**
 * @brief Get the Info object
 * 
*/
std::map<int, struct client_info> Client::getInfo()
{
	return (_clients);
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
		if (_server->getPollFds()[i].fd == client_fd)
		{
			_server->getPollFds()[i] = _server->getPollFds()[1];
			break;
		}
	}
	_clients.erase(client_fd);
}

/**
 * @brief Handles a client message
 * 
 * @param client_fd Client file descriptor
*/
void Client::handleClientMessage(int client_fd)
{
    char buffer[1024];
    int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	std::cout << "bytes_received: " << bytes_received << std::endl;
    if (bytes_received < 0) // Client disconnected
    {
        std::cout << "Client " << client_fd << " disconnected." << std::endl;
        close(client_fd);
        removeClient(client_fd);
		close(_server->getSock());
        return;
    }
    buffer[bytes_received] = '\0';
    std::cout << "Received from client " << client_fd << ": " << buffer << std::endl;
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
	std::istringstream iss(buffer);
	std::string command, param;
	iss >> command >> param;
	if (command != "PASS")
	{
		send(client_fd, "ERROR :You must authenticate with PASS first\r\n", 46, 0);
		return;
	}
	else
	{
		if (_clients[client_fd].isAuthenticated)
		{
			send(client_fd, "ERROR: Already authenticated\r\n", 30, 0);
			return;
		}
		if (param == _server->getPwd())
		{
			_clients[client_fd].isAuthenticated = true;
			send(client_fd, "Password accepted. Please send NICK and USER.\r\n", 46, 0);
		}
		else
		{
			send(client_fd, "ERROR :Incorrect password\r\n", 27, 0);
			close(client_fd);
			removeClient(client_fd);
		}
	}
}


/**
 * @brief Accepts a client connection
 * 
*/
void Client::acceptClient(int client_fd)
{
	socklen_t client_addr_len = sizeof(_client_adrr);
	client_fd = accept(_server->getSock(), (struct sockaddr *)&_client_adrr, &client_addr_len);
	if(client_fd < 0)
	{
		std::cerr << "Error: Client connection failed" << std::endl;
		close(_server->getSock());
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
		close(_server->getSock());
		return;
	}
	buffer[bytes_received] = '\0';
	std::string message(buffer);
	std::istringstream iss(message);
	std::string command, param;
	iss >> command >> param;

	if(command == "PASS")
	{
		if (_clients[client_fd].isAuthenticated)
        {
            send(client_fd, "ERROR :Already authenticated\r\n", 30, 0);
            return;
        }

        if (param == _server->getPwd()) 
        {
            _clients[client_fd].isAuthenticated = true;
            send(client_fd, "Password accepted. Please send NICK and USER.\r\n", 46, 0);
        }
        else
        {
            send(client_fd, "ERROR :Incorrect password\r\n", 27, 0);
            close(client_fd);
            removeClient(client_fd);
        }
    }
    else if (!_clients[client_fd].isAuthenticated)
    {
        send(client_fd, "ERROR :You must authenticate with PASS first\r\n", 46, 0);
    }
}