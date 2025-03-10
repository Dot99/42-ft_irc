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
#include "Commands.hpp"

class IrcServer;
class Commands;

class Client
{
	private:
		IrcServer &_server;
		struct sockaddr_in _client_adrr;
		std::string _nick;
		std::string _user;
		bool _isAuthenticated;
	public:
		Client(IrcServer &server);
		Client(const Client &rhs);
		Client &operator=(const Client &rhs);
		virtual ~Client();
		void setNick(std::string nick);
		void setUser(std::string user);
		void setAuthenticated(bool auth);
		std::string getNick();
		std::string getUser();
		bool getAuthenticated();
		int acceptClient(int client_fd);
		void handleClientMessage(int client_fd, Commands &commands);
		void removeClient(int client_fd);
		std::map<int, struct client_info> &getInfo();
};