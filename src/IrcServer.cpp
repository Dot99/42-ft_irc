/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 11:14:16 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/10 11:12:17 by gude-jes         ###   ########.fr       */
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
 * @brief Set the Client object
 * 
 * @param client Client
*/
void IrcServer::addChannel(Channel *channel)
{
	_channels.push_back(channel);
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
	addChannel(channel);
}

void IrcServer::run(Client &client, Commands &commands)
{
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
			// std::cout << "bater em sem abrigos" << std::endl;
			if (_poll_fds[i].revents & POLLIN) 
            {
                if (_poll_fds[i].fd == _socket)
				{
					int client_fd = client.acceptClient(_poll_fds[i].fd);
					if (client_fd < 0)
					{
						std::cerr << "Error accepting client" << std::endl;
						return;
					}
					struct pollfd newPoll;
					newPoll.fd = client_fd;
					newPoll.events = POLLIN;
					newPoll.revents = 0;
					_poll_fds.push_back(newPoll);
				}
                else
                	client.handleClientMessage(_poll_fds[i].fd, commands);
            }	
		}
	}
}