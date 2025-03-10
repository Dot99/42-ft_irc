/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 10:34:13 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/10 10:26:56 by gude-jes         ###   ########.fr       */
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

class IrcServer
{
	private:
		int _port;
		std::string _pwd;
		int _socket;
		std::map<int, std::string> _clients;
		struct sockaddr_in _server_addr;
		struct sockaddr_in _client_adrr;
		struct pollfd _poll_fds[1];
	public:
		IrcServer();
		IrcServer(const std::string args[]);
		IrcServer &operator=(const IrcServer &rhs);
		~IrcServer();
		void startServer();
		void acceptClient();
		void run();
};