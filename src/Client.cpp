/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 10:26:01 by gude-jes          #+#    #+#             */
/*   Updated: 2025/04/08 09:11:50 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "IrcServer.hpp"

/**
 * @brief Construct a new Client:: Client object
 * @param server IrcServer object
*/
Client::Client(IrcServer &server) : _server(server)
{
	_isAuthenticated = false;
	_welcome_sent = false;
	_passwordVerified = false;
};

/**
 * @brief Copy assignment operator
 * 
 * @param rhs Client object to copy
 * @return Client& Reference to the current object
*/
Client &Client::operator=(const Client &rhs)
{
	if (this != &rhs)
	{
		_nick = rhs._nick;
		_user = rhs._user;
		_host = rhs._host;
		_real_name = rhs._real_name;
		fd = rhs.fd;
		_welcome_sent = rhs._welcome_sent;
		_isAuthenticated = rhs._isAuthenticated;
	}
	return *this;
}
/**
 * @brief Copy constructor
 * 
 * @param src Client object to copy
*/
Client::Client(const Client &src) : _server(src._server)
{
	_nick = src._nick;
	_user = src._user;
	_host = src._host;
	_real_name = src._real_name;
	fd = src.fd;
	_welcome_sent = src._welcome_sent;
	_isAuthenticated = src._isAuthenticated;
}

/**
 * @brief Destroy the Client:: Client object 
*/
Client::~Client()
{
    for (size_t i = 0; i < _channels.size(); i++)
    {
        if (_channels[i])
        {
            _channels[i]->removeUser(this);
        }
    }
    _channels.clear();
}

/**
 * @brief Removes a client
 * 
 * @param client_fd Client file descriptor
*/
void Client::removeClient(int client_fd)
{
	for (size_t i = 0; i < 1; i++)
	{
		if (_server.getPollFds(i).fd == client_fd)
		{
			_server.getPollFds(i) = _server.getPollFds(1);
			break;
		}
	}
}

/**
 * @brief Handles a channel message
 * 
 * @param client_fd Client file descriptor
 * @param restOfCommand Rest of the command
*/
void Client::handleChannelMessage(int client_fd, const std::string restOfCommand)
{
	std::string Name, message, msg;
	size_t msgPos, hashPos;
	int isCHannel = 0;

	hashPos = restOfCommand.find("#");
	if (hashPos != std::string::npos)
		isCHannel = 1;
	
	msgPos = restOfCommand.find(":");
	if (msgPos == std::string::npos || msgPos + 1 >= restOfCommand.size())
		return;
	if (isCHannel)
		Name = restOfCommand.substr(hashPos, msgPos - hashPos);
	else
		Name = restOfCommand.substr(0, msgPos);
	message = restOfCommand.substr(msgPos + 1);
	if (message.empty()){
		return;
	}
	Name = clean_input(Name, SPACES);
	std::transform(Name.begin(), Name.end(), Name.begin(), ::tolower);
	msg = ":" + getNick() + " PRIVMSG " + Name + " :" + message + "\r\n";
	if (isCHannel)
	{
		for(size_t i = 0; i < _channels.size(); i++)
		{
			if (_channels[i]->getName() == Name)
			{
				std::vector<Client *> users = _channels[i]->getUsers();
				for (size_t j = 0; j < users.size(); j++)
				{
					if (users[j]->getFd() != client_fd)
						sendClientMsg(users[j]->getFd(), msg.c_str());
				}
			}
			return;
		}
	}
	else
	{
		Client *user = _server.getUserByNick(Name);
		if (user)
			sendClientMsg(user->getFd(), msg.c_str());
	}
}

/**
 * @brief Handles a client message
 * 
 * @param client_fd Client file descriptor
*/
void Client::handleClientMessage(int client_fd)
{
	static std::map<int, std::string> partialBuffers; // Store partial input for each client
	char buffer[1024];
	int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_received < 0) // Error in receiving
	{
		std::cerr << "Error receiving data from client " << client_fd << "." << std::endl;
		return;
	}
	else if (bytes_received == 0) // Client disconnected
	{
		std::cout << "Client " << client_fd << " disconnected." << std::endl;
		close(client_fd);
		removeClient(client_fd);
		return;
	}

	buffer[bytes_received] = '\0'; // Null-terminate the buffer
	partialBuffers[client_fd] += buffer; // Append received data to the client's partial buffer

	size_t newlinePos;
	while ((newlinePos = partialBuffers[client_fd].find('\n')) != std::string::npos)
	{
		// Extract the complete command up to the newline
		std::string command = partialBuffers[client_fd].substr(0, newlinePos);
		partialBuffers[client_fd].erase(0, newlinePos + 1); // Remove the processed command

		// Process the complete command
		_server.parseCommand(client_fd, command.c_str());
	}
}

/**
 * @brief Accepts a client connection
 * @param client_fd Client file descriptor
*/
int Client::acceptClient(int client_fd)
{
	socklen_t client_addr_len = sizeof(_client_adrr);
	client_fd = accept(_server.getSock(), (struct sockaddr *)&_client_adrr, &client_addr_len);
	if(client_fd < 0)
	{
		std::cerr << "Error: Client connection failed" << std::endl;
		close(_server.getSock());
		return(-1);
	}
	setHost(std::string(inet_ntoa(_client_adrr.sin_addr)));
	setFd(client_fd);
	std::string input = readLine(client_fd, 512);
	if(input.empty() || input.find("CAP LS") == std::string::npos)
	{
		std::cerr << "Error: No input from client: " << client_fd << std::endl;
		sendClientMsg(client_fd, "Leave NC and try again \r\n");
		close(client_fd);
		return (-1);
	}
	if(input.find("CAP LS") != std::string::npos)
	{
		while (!getAuthenticated())
			validateUser(client_fd);	
		return(client_fd);
	}
	return (-1);
}

/**
 * @brief Validates the user
 * 
 * @param client_fd Client file descriptor
*/
void Client::validateUser(int client_fd)
{
	std::string input = readLine(client_fd, 512);
	if (input.find("PASS") != std::string::npos)
			_server.parseCommand(client_fd, input);
	else if ((input.find("NICK")) != std::string::npos)
		_server.parseCommand(client_fd, input);
	else if (input.find("USER") != std::string::npos)
		_server.parseCommand(client_fd, input);
	else
		return;
}


/*---------------------------- GETTERS/SETTERS---------------------------- */




/**
 * @brief Set the Nick object
 * 
 * @return std::string Nickname
*/
void Client::setUser(std::string const &user)
{
	_user = user;
}

/**
 * @brief Set the Nick object
 * 
 * @return std::string Nickname
*/
void Client::setNick(std::string nick)
{
	_nick = nick;
}

/**
 * @brief Get the User Nickname
 * 
 * @return std::string Nickname
*/
std::string Client::getNick()
{
	return _nick;
}

/**
 * @brief Get the User Username
 * 
 * @return std::string User
*/
std::string Client::getUser()
{
	return _user;
}

/**
 * @brief Set the Client File Descriptor
 * 
 * @param fd File Descriptor
*/
void Client::setFd(int fd)
{
	this->fd = fd;
}

/**
 * @brief Set the Channel object
 * 
 * @param channel Channel
*/
void Client::addChannel(Channel *channel)
{
	if(std::find(_channels.begin(), _channels.end(), channel) == _channels.end())
	{
		_channels.push_back(channel);
	}
}

/**
 * @brief Remove the Channel object
 * 
 * @param Channel Channel
*/
void Client::removeChannel(Channel *Channel)
{
	_channels.erase(std::remove(_channels.begin(), _channels.end(), Channel), _channels.end());
}

/**
 * @brief Find a channel by name
 * 
 * @param name Channel name
 * @return int Index of the channel
*/
int Client::findChannel(std::string const &name)
{
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (_channels[i]->getName() == name)
			return (i);
	}
	return (-1);
}

/**
 * @brief Get the Client File Descriptor
 * 
 * @return int File Descriptor
*/
int Client::getFd()
{
	return fd;
}

/**
 * @brief Set the Authenticated object
 * 
 * @param auth If the client is authenticated
*/
void Client::setAuthenticated(bool auth)
{
	_isAuthenticated = auth;
}

/**
 * @brief Check if the client is authenticated
 * 
 * @return true If the client is authenticated
 * @return false If the client is not authenticated
*/
bool Client::getAuthenticated()
{
	return (_isAuthenticated);
}

/**
 * @brief Set the Welcome Sent object
 * 
 * @param welcome_sent If the welcome message was sent
*/
void Client::setWelcomeSent(bool welcome_sent)
{
	_welcome_sent = welcome_sent;
}

/**
 * @brief Get the Welcome Sent object
 * 
 * @return true If the welcome message was sent
 * @return false If the welcome message was not sent
*/
bool Client::getWelcomeSent()
{
	return (_welcome_sent);
}

/**
 * @brief Set the Host object
 * 
 * @param host Host
*/
void Client::setHost(std::string host)
{
	_host = host;
}

/**
 * @brief Get the Host object
 * 
 * @return std::string Host
*/
std::string Client::getHost()
{
	return _host;
}

/**
 * @brief Set the Real Name object
 * 
 * @param real_name Real Name
*/
void Client::setRealName(std::string real_name)
{
	_real_name = real_name;
}

/**
 * @brief Get the Real Name object
 * 
 * @return std::string Real Name
*/
std::string Client::getRealName()
{
	return _real_name;
}

/**
 * @brief Set the Password Verified object
 * 
 * @param password_verified If the password was verified
*/
void Client::setPasswordVerified(bool password_verified)
{
	_passwordVerified = password_verified;
}

/**
 * @brief Get the Password Verified object
 * 
 * @return true If the password was verified
 * @return false If the password was not verified
*/
bool Client::getPasswordVerified()
{
	return _passwordVerified;
}

/**
 * @brief Get the Channels object
 * 
 * @return std::vector<Channel *> Channels
*/
std::vector<Channel *> Client::getChannels()
{
	return _channels;
}