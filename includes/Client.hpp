/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 10:25:36 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/10 12:56:55 by gude-jes         ###   ########.fr       */
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

struct client_info
{
	bool isAuthenticated;
	std::string nick;
};

class Client
{
	private:
		IrcServer* _server;
		std::map<int, struct client_info> _clients;
		struct sockaddr_in _client_adrr;
	public:
		Client(IrcServer* server);
		Client(const Client &rhs);
		Client &operator=(const Client &rhs);
		virtual ~Client();
		void acceptClient(int client_fd);
		void handleClientMessage(int client_fd);
		void removeClient(int client_fd);
		std::map<int, struct client_info> getInfo();
};