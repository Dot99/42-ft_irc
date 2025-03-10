/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 10:34:13 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/10 11:11:42 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <string>
#include <poll.h>
#include <map>
#include <sstream>
#include "Client.hpp"

class Client;

class IrcServer
{
	private:
		Client* _client;
		int _port;
		std::string _pwd;
		int _socket;
		struct sockaddr_in _server_addr;
		struct pollfd _poll_fds[1];
	public:
		IrcServer(Client* client);
		IrcServer(const std::string args[], Client* client);
		IrcServer &operator=(const IrcServer &rhs);
		void setClient(Client* client);
		int getSock() const;
		std::string getPwd() const;
		struct pollfd *getPollFds(); 
		virtual ~IrcServer();
		void startServer();
		void run();
};