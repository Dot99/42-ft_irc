/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 10:25:36 by gude-jes          #+#    #+#             */
/*   Updated: 2025/04/08 09:19:09 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Utils.hpp"

/**
 * @brief Client class represents a client connected to the IRC server.
 * It contains information about the client's nickname, username, host,
 * real name, file descriptor, and authentication status.
 * It also provides methods to handle client messages, manage channels,
 * and validate user authentication.
 * @details
 * - The class contains private member variables to store the client properties.
 * - The constructor initializes the client with the server reference.
 * - The destructor is empty as there are no dynamic allocations.
 * - The class provides methods to validate user authentication, accept client connections,
 *  handle client messages, and manage channels.
 * - The class uses a vector to store the channels the client is part of.
 * - The class uses a string to store the client's nickname, username, host, and real name.
 * - The class uses a boolean to store the welcome message status and authentication status.
 * - The class uses a boolean to store the password verification status.
 * - The class uses a sockaddr_in structure to store the client's address.
 * - The class provides getters and setters for the client properties.
 * - The class provides methods to add and remove channels.
 * - The class provides methods to find a channel by name.
 * - The class provides methods to handle channel messages and client messages.
 * - The class provides methods to remove a client from the server.
 * - The class provides methods to set and get the client properties.
 * - The class provides methods to set and get the client file descriptor.
 * - The class provides methods to set and get the client nickname.
 * - The class provides methods to set and get the client username.
 * - The class provides methods to set and get the client host.
 * - The class provides methods to set and get the client real name.
 * - The class provides methods to set and get the client welcome message status.
 * - The class provides methods to set and get the client authentication status.
 * - The class provides methods to set and get the client password verification status.
 * - The class provides methods to set and get the client address.
 * - The class provides methods to set and get the client channels.
 * 
*/
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