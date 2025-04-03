/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 10:25:36 by gude-jes          #+#    #+#             */
/*   Updated: 2025/04/03 15:47:00 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Utils.hpp"

class Client
{
	private:
		IrcServer &_server;
		std::vector<Channel *> _channels;
		int fd;
		std::string _nick;
		std::string _user;
		std::string _host;
		std::string _real_name;
		bool _welcome_sent;
		bool _isAuthenticated;
		bool _passwordVerified;
		struct sockaddr_in _client_adrr;
	public:
		Client(IrcServer &server);
		Client(const Client &src);
		Client &operator=(const Client &rhs);
		virtual ~Client();

		void validateUser(int client_fd);
		int acceptClient(int client_fd);
		void handleChannelMessage(int client_fd, const std::string buffer);
		void handleClientMessage(int client_fd);
		void removeClient(int client_fd);
		int findChannel(std::string const &name);
		//-----------------GETTERS/SETTERS-----------------//
		void setAuthenticated(bool auth);
		void setUser(std::string const &user);
		void setNick(std::string nick);
		void setRealName(std::string real_name);
		void setHost(std::string host);
		void setFd(int fd);
		void addChannel(Channel *channel);
		void removeChannel(Channel *channel);
		void setWelcomeSent(bool welcome_sent);
		void setPasswordVerified(bool password_verified);
		std::vector<Channel *> getChannels();
		bool getPasswordVerified();
		bool getWelcomeSent();
		std::string getNick();
		std::string getUser();
		bool getAuthenticated();
		int getFd();
		std::string getHost();
		Channel *getChannel();
		std::string getRealName();
};