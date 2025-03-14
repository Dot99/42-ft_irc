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
#include "Utils.hpp"

class Client;
class Channel;

class IrcServer
{
	private:
		int _port;
		int _socket;
		std::string _pwd;
		struct sockaddr_in _server_addr;
		std::vector<struct pollfd> _poll_fds;
		std::vector<Channel *> _channels;
		std::vector<Client *> _users;
		Client *_user;
	public:
		IrcServer(const std::string args[]);
		IrcServer &operator=(const IrcServer &rhs);
		virtual ~IrcServer();

		void addChannel(Channel *channel);
		void addUser(Client *client);
		void startServer();
		void run();

		//-----------------COMMANDS-----------------//
		void leaveCommand(int client_fd);
		void usersCommand(int client_fd);
		void exitCommand(int client_fd);
		void nickCommand(int client_fd, std::string restOfCommand);
		void joinCommand(int client_fd, std::string restOfCommand);
		void listCommand(int client_fd, std::string restOfCommand);
		void kickCommand(int client_fd, std::string restOfCommand);
		void inviteCommand(int client_fd, std::string restOfCommand);
		void topicCommand(int client_fd, std::string restOfCommand);
		void modeCommand(int client_fd, std::string restOfCommand);
		void parseCommand(int client_fd, std::string command);

		//-----------------GETTERS/SETTERS-----------------//
		int getSock() const;
		std::string getPwd() const;
		struct pollfd &getPollFds(int i);
		std::vector<Channel *> getChannels() const;
		std::vector<Client *> getUsers() const;
		Channel * getChannelByName(std::string name);
		Client * getUserByNick(std::string nick);
		Client * getUserFd(int fd);
		void setPollFds(int i, int fd, short int revents); 
};