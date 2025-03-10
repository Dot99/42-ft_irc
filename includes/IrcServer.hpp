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
#include <vector>
#include "Client.hpp"
#include "Commands.hpp"
#include "Utils.hpp"

class Client;
class Commands;

class IrcServer
{
	private:
		int _port;
		std::string _pwd;
		int _socket;
		struct sockaddr_in _server_addr;
		std::vector<struct pollfd> _poll_fds;
	public:
		IrcServer(Client* client);
		IrcServer(const std::string args[]);
		IrcServer &operator=(const IrcServer &rhs);
		int getSock() const;
		std::string getPwd() const;
		struct pollfd &getPollFds(int i);
		void setPollFds(int i, int fd, short int revents); 
		void setClient(Client* client);
		virtual ~IrcServer();
		void startServer();
		void run(Client &client, Commands &commands);
};