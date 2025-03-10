/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 11:14:16 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/07 09:29:02 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcServer.hpp"


/**
 * @brief Construct a new Default Irc Server:: Irc Server object
*/
IrcServer::IrcServer()
{
}

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
	_pwd = args[1];
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
 * @brief Removes a client
 * 
 * @param client_fd Client file descriptor
*/
void IrcServer::removeClient(int client_fd)
{
	for (size_t i = 0; i < 1; i++)
	{
		if (_poll_fds[i].fd == client_fd)
		{
			_poll_fds[i] = _poll_fds[1];
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
void IrcServer::handleClientMessage(int client_fd)
{
    char buffer[1024];
    int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	std::cout << "bytes_received: " << bytes_received << std::endl;
    if (bytes_received < 0) // Client disconnected
    {
        std::cout << "Client " << client_fd << " disconnected." << std::endl;
        close(client_fd);
        removeClient(client_fd);
		close(_socket);
        return;
    }
    buffer[bytes_received] = '\0';
    std::cout << "Received from client " << client_fd << ": " << buffer << std::endl;
    // TODO: Parse and process IRC commands here
}


/**
 * @brief Accepts a client connection
 * 
*/
void IrcServer::acceptClient()
{
	socklen_t client_addr_len = sizeof(_client_adrr);
	int client_fd = accept(_socket, (struct sockaddr *)&_client_adrr, &client_addr_len);
	if(client_fd < 0)
	{
		std::cerr << "Error: Client connection failed" << std::endl;
		close(_socket);
		//exit(1);
	}
	std::cout << "Client connected" << std::endl;
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
	// Bind the socket to an IP / port
	// sockaddr_in is a structure containing an internet address
	// sin_family: AF_INET - IPv4
	// sin_port: Port number (htons to convert to big endian)
	// sin_addr: IP address (INADDR_ANY to bind to all local interfaces)
	memset(&_server_addr, 0, sizeof(_server_addr));
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(_port);
	_server_addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(_socket, (sockaddr *)&_server_addr, sizeof(_server_addr)))
	{
		std::cerr << "Error: Bind failed" << std::endl;
		close(_socket);
		//exit(1);
	}
	if(listen(_socket, 5) < 0)
	{
		std::cerr << "Error: Listen failed" << std::endl;
		close(_socket);
		//exit(1);
	}
	std::cout << "Server started on port: " << _port << std::endl;
}

void IrcServer::run()
{
	while (1)
	{
		int event_count = poll(_poll_fds, 3, 0);
        if (event_count == -1)
        {
            std::cerr << "Error: poll() failed" << std::endl;
            break;
        }
        for (size_t i = 0; i < 3; i++)
        {
            if (_poll_fds[i].revents & POLLIN) 
            {
                if (_poll_fds[i].fd == _socket)
					acceptClient();
                else
                    handleClientMessage(_poll_fds[i].fd);
            }
		}
	}
	
}