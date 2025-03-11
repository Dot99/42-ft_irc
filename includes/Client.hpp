/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 10:25:36 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/11 12:53:44 by gude-jes         ###   ########.fr       */
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
#include "Utils.hpp"


class IrcServer;
class Commands;

class Client
{
	private:
		IrcServer &_server;
		struct sockaddr_in _client_adrr;
		std::map<std::string, std::string> _user;
		bool _isAuthenticated;
	public:
		Client(IrcServer &server);
		virtual ~Client();
		void setAuthenticated(bool auth);
		void setUser(std::string nick, std::string pass);
		std::map<std::string, std::string>::iterator getNick(std::string nick);
		std::string getUserPass(std::string nick);
		bool getAuthenticated();
		int acceptClient(int client_fd);
		void handleClientMessage(int client_fd, Commands &commands);
		void removeClient(int client_fd);
		int checkPwd(int client_fd);
};