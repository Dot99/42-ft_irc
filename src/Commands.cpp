/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 12:19:16 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/12 11:57:25 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Commands.hpp"

Commands::Commands(IrcServer &server): _server(server)
{
}

Commands::~Commands()
{
}

/**
 * @brief Command to change the nickname of the user
 * @param client_fd File descriptor of the client
*/
void Commands::nickCommand(int client_fd, std::string restOfCommand)
{
	std::string inputNick = restOfCommand;
	std::string msg;
	if(inputNick.empty())
	{
		send(client_fd, "Invalid nickname\n", 17, 0);
	}
	_server.getUserFd(client_fd)->setUser(inputNick, _server.getUserFd(client_fd)->getPass());
	msg = "User " + inputNick + " changed nickname\n";
	send(client_fd, msg.c_str(), msg.length(), 0);
	std::cout << msg;
}

/**
 * @brief Command to join a channel
 * @param client_fd File descriptor of the client
*/
void Commands::joinCommand(int client_fd, std::string restOfCommand)
{
	std::string inputChannel = restOfCommand;
	bool channelExists = false;
	std::string msg;
	Client *user = _server.getUserFd(client_fd);
	if(inputChannel.empty())
	{
		send(client_fd, "Invalid channel\n", 16, 0);
	}
	for (size_t i = 0; i < _server.getChannels().size(); i++)
	{
		if (_server.getChannels()[i]->getName() == inputChannel)
		{
			// if (_server.getChannels()[i]->getPassword().empty())
			// {
			// 	_server.getChannels()[i]->addUser(&_client);
			// 	channelExists = true;
			// 	break;
			// }
			// else
			// {
			// 	send(client_fd, "Channel has password\n", 21, 0);
			// 	break;
			// }
			channelExists = true;
			for (size_t j = 0; j < _server.getChannels()[i]->getUsers().size(); j++)
			{
				if (_server.getChannels()[i]->getUsers()[j]->getNick() !=user->getNick())
				{
					_server.getChannels()[i]->addUser(user);
					user->setChannel(_server.getChannels()[i]);
					msg = "User " + user->getNick() + " added to channel: " + _server.getChannels()[i]->getName() + "\n";
					send(client_fd, msg.c_str(), msg.length(), 0);
					std::cout << msg;
					break;
				}
			}
			break;
		}
	}
	if (!channelExists)
	{
		Channel *channel = new Channel(inputChannel);
		_server.addChannel(channel);
		_server.getChannels()[_server.getChannels().size() - 1]->addUser(user);
		user->setChannel(_server.getChannels()[_server.getChannels().size() - 1]);
		msg = "Channel " + _server.getChannels()[_server.getChannels().size() - 1]->getName() + " created and user " + user->getNick() + " added\n";
		send(client_fd, msg.c_str(), msg.length(), 0);
		std::cout << msg;
	}
	//TODO:Check if channel exists and if it has pwd
}

/**
 * @brief Command to leave a channel
 * @param client_fd File descriptor of the client
*/
void Commands::leaveCommand(int client_fd)
{
	//TODO: Leave command have a message to send to the channel
	std::string inputChannel = readLine(client_fd, 200); //(200) Max Channel characters
	Client *user = _server.getUserFd(client_fd);
	if(inputChannel.empty())
	{
		send(client_fd, "Invalid channel\n", 16, 0);
	}
	if (user->getChannel())
	{
		user->getChannel()->removeUser(user);
		user->setChannel(NULL);
	}
	else
		send(client_fd, "User is not in a channel\n", 25, 0);
	//TODO: Check if user is in channel
	// If true
	// Leave channel
	// Else
	// Send error message
}

/**
 * @brief Command to list all channels
 * @param client_fd File descriptor of the client
*/
void Commands::listCommand(int client_fd, std::string restOfCommand)
{
	if(restOfCommand.empty())
	{
		for (size_t i = 0; i < _server.getChannels().size(); i++)
		{
			send(client_fd, "Channel: ", 9, 0);
			send(client_fd, _server.getChannels()[i]->getName().c_str(), _server.getChannels()[i]->getName().length(), 0);
			send(client_fd, "\n", 1, 0);
		}
		
	}
	else
	{
		//TODO: List all channels with the full/part name of the channel
	}
}

void Commands::usersCommand(int client_fd)
{
	size_t i = 0;
	std::string msg;
	for (;i < _server.getChannels().size(); i++)
	{
		if (_server.getChannels()[i]->getUserFd(client_fd))
			break;
	}
	for (size_t j = 0; j < _server.getChannels()[i]->getUsers().size(); j++)
	{
		msg = "Channel: " + _server.getChannels()[i]->getName() + "\nUser: " + _server.getChannels()[i]->getUsers()[j]->getNick() + "\n";
		send(client_fd, msg.c_str(), msg.length(), 0);
	}
	//TODO: List all users in the channel
}

void Commands::exitCommand(int client_fd)
{
	(void)client_fd;
	//TODO: Leave channel and close connection
}

void Commands::kickCommand(int client_fd, std::string restOfCommand)
{
	std::string inputNick = restOfCommand;
	if(inputNick.empty())
	{
		send(client_fd, "Invalid nickname\n", 17, 0);
	}
	//TODO: Check if user is in channel
	// If true
	// Kick user
	// Else
	// Send error message
}

void Commands::inviteCommand(int client_fd, std::string restOfCommand)
{
	std::string inputNick = restOfCommand;
	if(inputNick.empty())
	{
		send(client_fd, "Invalid nickname\n", 17, 0);
	}
	//TODO: Invite user to channel
}

void Commands::topicCommand(int client_fd, std::string restOfCommand)
{
	std::string inputTopic = restOfCommand;
	if(inputTopic.empty())
	{
		send(client_fd, "Invalid topic\n", 14, 0);
	}
	//TODO: Change topic of the channel
}

void Commands::modeCommand(int client_fd, std::string restOfCommand)
{
	std::string inputMode = restOfCommand;
	if(inputMode.empty())
	{
		send(client_fd, "Invalid mode\n", 13, 0);
	}
	//TODO: Change mode of the channel
}

void Commands::parseCommand(int client_fd, std::string command)
{
	std::string commands[10] = {"/nick", "/join", "/leave", "/list", "/users", "/exit", "/kick",  "/invite", "/topic", "/mode"};
	int i = 0;
    std::string foundCommand;
    std::string restOfCommand;

    for(; i < 10; i++)
    {
        size_t pos = command.find(commands[i]);
        if(pos != std::string::npos)
        {
            foundCommand = commands[i];
            restOfCommand = command.substr(pos + commands[i].length());
			restOfCommand = clean_input(restOfCommand);
            break;
        }
    }
	switch(i)
	{
		case 0:
			nickCommand(client_fd, restOfCommand);
			break;
		case 1:
			joinCommand(client_fd, restOfCommand);
			break;
		case 2:
			leaveCommand(client_fd);
			break;
		case 3:
			listCommand(client_fd, restOfCommand);
			break;
		case 4:
			usersCommand(client_fd);
			break;
		case 5:
			exitCommand(client_fd);
			break;
		case 6:
			kickCommand(client_fd, restOfCommand);
			break;
		case 7:
			inviteCommand(client_fd, restOfCommand);
			break;
		case 8:
			topicCommand(client_fd, restOfCommand);
			break;
		case 9:
			modeCommand(client_fd, restOfCommand);
			break;
		default:
			break;
	}
}