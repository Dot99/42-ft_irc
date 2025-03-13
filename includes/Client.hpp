/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 10:25:36 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/13 08:55:29 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Utils.hpp"


class IrcServer;
class Channel;

class Client
{
	private:
		IrcServer &_server;
		Channel *_channel;
		int fd;
		std::string _nick;
		std::string _pass;
		bool _isAuthenticated;
		bool _isOperator;
		struct sockaddr_in _client_adrr;
	public:
		Client(IrcServer &server);
		virtual ~Client();

		void validateUser(int client_fd);
		int acceptClient(int client_fd);
		void hanleChannelMessage(int client_fd, char *buffer);
		void handleClientMessage(int client_fd);
		void removeClient(int client_fd);
		int checkPwd(int client_fd);

		//-----------------GETTERS/SETTERS-----------------//
		void setAuthenticated(bool auth);
		void setOperator(bool op);
		void setUser(std::string nick, std::string pass);
		std::string getNick();
		std::string getPass();
		bool getAuthenticated();
		bool getOperator();
		int getFd();
		void setFd(int fd);
		void setChannel(Channel *channel);
		Channel *getChannel();
};