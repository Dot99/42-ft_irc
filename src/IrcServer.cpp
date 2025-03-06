/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 11:14:16 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/03 12:23:50 by gude-jes         ###   ########.fr       */
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
	_port = std::stoi(args[0]);
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
	if(_socket == -1)
	{
		std::cerr << "Error: Socket creation failed" << std::endl;
		//exit(1);
	}
}

void IrcServer::run()
{

}