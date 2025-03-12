/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 12:18:45 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/12 11:14:16 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Utils.hpp"

class Client;
class IrcServer;
class Commands
{
	private:
		IrcServer &_server;
		Client &_client;
	public:
		Commands(IrcServer &server, Client &client);
		~Commands();
		void nickCommand(int client_fd, std::string restOfCommand);
		void joinCommand(int client_fd, std::string restOfCommand);
		void leaveCommand(int client_fd);
		void listCommand(int client_fd, std::string restOfCommand);
		void usersCommand(int client_fd);
		void exitCommand(int client_fd);
		void kickCommand(int client_fd, std::string restOfCommand);
		void inviteCommand(int client_fd, std::string restOfCommand);
		void topicCommand(int client_fd, std::string restOfCommand);
		void modeCommand(int client_fd, std::string restOfCommand);
		void parseCommand(int client_fd, std::string command);
};