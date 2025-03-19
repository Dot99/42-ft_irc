/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 10:25:36 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/18 15:31:14 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Utils.hpp"

class Client
{
	private:
		IrcServer &_server;
		Channel *_channel;
		int fd;
		std::string _nick;
		std::string _user;
		bool _isAuthenticated;
		bool _isOperator;
		struct sockaddr_in _client_adrr;
	public:
		Client(IrcServer &server);
		virtual ~Client();

		void validateUser(int client_fd, std::string input);
		int acceptClient(int client_fd);
		void handleChannelMessage(int client_fd, const std::string buffer);
		void handleClientMessage(int client_fd);
		void removeClient(int client_fd);
		int checkPwd(int client_fd);

		//-----------------GETTERS/SETTERS-----------------//
		void setAuthenticated(bool auth);
		void setOperator(bool op);
		void setUser(std::string user);
		void setNick(std::string nick);
		std::string getNick();
		std::string getUser();
		bool getAuthenticated();
		bool getOperator();
		int getFd();
		void setFd(int fd);
		void setChannel(Channel *channel);
		Channel *getChannel();
};