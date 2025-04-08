/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 10:34:13 by gude-jes          #+#    #+#             */
/*   Updated: 2025/04/08 09:19:03 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Utils.hpp"

const std::string SERVER_NAME = "Club Penguin";

/**
 * @brief IrcServer class represents the IRC server.
 * It contains information about the server port, socket, password,
 * current time, server address, poll file descriptors, channels, and users.
 * It also provides methods to add and remove channels and users,
 * start the server, and run the main loop.
 * @details
 * - The class contains private member variables to store the server properties.
 * - The constructor initializes the server with the given arguments.
 * - The destructor is empty as there are no dynamic allocations.
 * - The class provides methods to add and remove channels and users.
 * - The class provides methods to start the server and run the main loop.
 * - The class provides methods to handle client commands such as part, exit, nick,
 *  join, list, kick, invite, topic, mode, pass, user, quit, and who.
 * - The class provides getters and setters for the server properties.
 * - The class provides methods to parse commands and handle client messages.
 * - The class uses a vector to store the channels and users.
 * - The class uses a string to store the server password and current time.
 * - The class uses a sockaddr_in structure to store the server address.
 * - The class uses a vector of pollfd structures to manage file descriptors.
 * - The class uses a pointer to the Client class to represent the current user.
 * - The class uses a vector of strings to store command arguments.
 * - The class uses a string to store the command arguments.
 * - The class uses a string to store the command rest of the arguments.
 * - The class uses a string to store the command name.
 * 
*/
class IrcServer
{
	private:
		int _port;
		int _socket;
		std::string _pwd;
		std::string _now;
		struct sockaddr_in _server_addr;
		std::vector<struct pollfd> _poll_fds;
		std::vector<Channel *> _channels;
		std::vector<Client *> _users;
		std::vector<std::string> _args;
		Client *_user;
	public:
		IrcServer(char **argv, int argc);
		IrcServer &operator=(const IrcServer &rhs);
		virtual ~IrcServer();

		void addChannel(Channel *channel);
		void addUser(Client *client);
		void removeUser(Client *client);
		void removeChannelByName(std::string const &name);
		void startServer();
		void run();

		//-----------------COMMANDS-----------------//
		void partCommand(int client_fd, std::string restOfCommand);
		void exitCommand(int client_fd);
		void nickCommand(int client_fd, std::string restOfCommand);
		void joinCommand(int client_fd, std::string restOfCommand);
		void listCommand(int client_fd, std::string restOfCommand);
		void kickCommand(int client_fd, std::string restOfCommand);
		void inviteCommand(int client_fd, std::string restOfCommand);
		void topicCommand(int client_fd, std::string restOfCommand);
		void modeCommand(int client_fd, std::string restOfCommand);
		void parseCommand(int client_fd, std::string command);
		void passCommand(int client_fd, std::string restOfCommand);
		void userCommand(int client_fd, std::string restOfCommand);
		void quitCommand(int client_fd, std::string restOfCommand);
		void whoCommand(int client_fd, std::string restOfCommand);
		//-----------------GETTERS/SETTERS-----------------//
		void setArgs(const std::vector<std::string> &args);
		void setPollFds(int i, int fd, short int revents); 
		void setPwd(std::string const &pwd);
		int getSock() const;
		std::string getPwd() const;
		struct pollfd &getPollFds(int i);
		std::vector<Channel *> getChannels() const;
		std::vector<Client *> getUsers() const;
		Channel * getChannelByName(std::string name);
		Client * getUserByNick(std::string nick);
		Client * getUserFd(int fd);
};