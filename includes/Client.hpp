/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 10:25:36 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/10 10:30:15 by gude-jes         ###   ########.fr       */
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
#include "IrcServer.hpp"

class IrcServer;

class Client
{
	private:
		IrcServer &_server;
		std::map<int, std::string> _clients;
		struct sockaddr_in _client_adrr;
	public:
		Client(IrcServer &server);
		Client(const Client &rhs);
		Client &operator=(const Client &rhs);
		~Client();
		void acceptClient(int client_fd);
		void handleClientMessage(int client_fd);
		void removeClient(int client_fd);
};