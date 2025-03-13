/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 12:19:16 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/13 09:26:05 by gude-jes         ###   ########.fr       */
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
	//TODO: Check if nickname is already in use and it's uppercase and lowercase letters. Ex: DOT it's the same nick as dot
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
	if(inputChannel.empty())
	{
		send(client_fd, "Invalid channel\n", 16, 0);
	}
	for (size_t i = 0; i < _server.getChannels().size(); i++)
	{
		if ((_server.getChannels()[i]->getName() == inputChannel) && _user->getChannel() == NULL)
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
				if (_server.getChannels()[i]->getUsers()[j]->getNick() != _user->getNick())
				{
					_server.getChannels()[i]->addUser(_user);
					_user->setChannel(_server.getChannels()[i]);
					msg = "User " + _user->getNick() + " added to channel: " + _server.getChannels()[i]->getName() + "\n";
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
		_server.getChannels()[_server.getChannels().size() - 1]->addUser(_user);
		_user->setChannel(_server.getChannels()[_server.getChannels().size() - 1]);
		msg = "Channel " + _server.getChannels()[_server.getChannels().size() - 1]->getName() + " created and user " + _user->getNick() + " added\n";
		//Since the user is the first in the channel, it is the operator
		//TODO: Set user as operator
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
	std::string inputChannel = readLine(client_fd, 200); //(200) Max Channel characters
	if(inputChannel.empty())
	{
		send(client_fd, "Invalid channel\n", 16, 0);
	}
	if (_user->getChannel())
	{
		sendClientMsg(client_fd, "User left channel\n", 0);
		_user->getChannel()->removeUser(_user);
		_user->setChannel(NULL);
	}
	else
		send(client_fd, "You are not in a channel\n", 25, 0);
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
		//TODO: List all channels with the full/part name of the channel (Deal with wildcards?)
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
	//TODO: Pretty print of the users in the channel
}

void Commands::exitCommand(int client_fd)
{
	(void)client_fd;
	_user->getChannel()->removeUser(_user);
	_user->setChannel(NULL);
	//TODO: Close connection with the client
}

void Commands::kickCommand(int client_fd, std::string restOfCommand)
{
	if(_user->getOperator() == false)
	{
		send(client_fd, "Not allowed", 11, 0);
		return ;
	}
	std::string inputNick = restOfCommand;
	if(inputNick.empty())
	{
		send(client_fd, "Invalid nickname\n", 17, 0);
	}
	if(_user->getChannel())
	{
		for (size_t i = 0; i < _user->getChannel()->getUsers().size(); i++)
		{
			if (_user->getChannel()->getUsers()[i]->getNick() == inputNick)
			{
				_user->getChannel()->removeUser(_user->getChannel()->getUsers()[i]);
				break;
			}
		}
	}
	else
		send(client_fd, "No user found\n", 14, 0);
}

void Commands::inviteCommand(int client_fd, std::string restOfCommand)
{
	//TODO: Check if channel is invite only
	// If true
	// 	Check if user is operator
	// 	If true
	// 		Invite user to channel
	// 	Else
	// 		Send error message
	// Else
	// 	Invite user to channel
	std::string inputNick = restOfCommand;
	if(inputNick.empty())
	{
		send(client_fd, "Invalid nickname\n", 17, 0);
	}
	//TODO: Invite user to channel
}

void Commands::topicCommand(int client_fd, std::string restOfCommand)
{
	if(_user->getOperator() == false)
	{
		send(client_fd, "Not allowed", 11, 0);
		return ;
	}
	std::string inputTopic = restOfCommand;
	if(inputTopic.empty())
	{
		//TODO: Show to op the topic of the channel
	}
	else
	{
		//TODO: Change topic of the channel
	}
}

void Commands::modeCommand(int client_fd, std::string restOfCommand)
{
	if(_user->getOperator() == false)
	{
		send(client_fd, "Not allowed", 11, 0);
		return ;
	}
	std::string inputMode = restOfCommand;
	if(inputMode.empty())
	{
		send(client_fd, "Invalid mode\n", 13, 0);
	}
	else
	{
		if(inputMode == "i")
		{
			//TODO: Change mode of the channel to invite only
		}
		else if (inputMode == "t")
		{
			//TODO: Change restrictions of the TOPIC command
		}
		//TODO: else if inputMode "k" change channel password with the rest of the command
	}
	//TODO: Change mode of the channel
}

void Commands::parseCommand(int client_fd, std::string command)
{
	std::string commands[10] = {"/nick", "/join", "/leave", "/list", "/users", "/exit", "/kick",  "/invite", "/topic", "/mode"};
	int i = 0;
    std::string foundCommand;
    std::string restOfCommand;
	_user = _server.getUserFd(client_fd);
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
			send(client_fd, "Invalid command\n", 16, 0);
			break;
	}
}