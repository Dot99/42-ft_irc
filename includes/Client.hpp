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
#include "Utils.hpp"


class IrcServer;
class Commands;
class Channel;

class Client
{
	private:
		IrcServer &_server;
		Channel *_channel;
		int fd;
		std::string _nick;
		std::string _pass;
		struct sockaddr_in _client_adrr;
		std::map<std::string, std::string> _user;
		bool _isAuthenticated;
	public:
		Client(IrcServer &server);
		virtual ~Client();
		void setAuthenticated(bool auth);
		void setUser(std::string nick, std::string pass);
		std::string getNick();
		std::string getPass();
		bool getAuthenticated();
		int getFd();
		void setFd(int fd);
		void setChannel(Channel *channel);
		Channel *getChannel();
		void validateUser(int client_fd);
		void addUserData(std::string nick, std::string pass);
		void removeUserData(std::string nick);
		int acceptClient(int client_fd);
		void handleClientMessage(int client_fd, Commands &commands);
		void removeClient(int client_fd);
		int checkPwd(int client_fd);
};