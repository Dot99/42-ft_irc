/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 12:19:16 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/12 11:55:39 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Commands.hpp"

Commands::Commands(IrcServer &server, Client &client): _server(server), _client(client)
{
	(void)_server;
}

Commands::~Commands()
{
}

void Commands::validateUser(int client_fd)
{
	std::string inputNick = readLine(client_fd, 9); //(9) Max Nickname characters
	if(inputNick.empty())
	{
		//TODO: Send user to nick again due to error
		send(client_fd, "Invalid nickname\n", 17, 0);
		close(client_fd);
		//return (-1);
	}
	std::string cleaned_input_nick = cleanInput(inputNick);
	if(cleaned_input_nick == _client.getNick(cleaned_input_nick)->first)
	{
		send(client_fd, "PASS:\n", 5, 0);
		std::string inputPwd = readLine(client_fd, 510); //(510) Max pwd characters
		if(inputPwd.empty())
		{
			//TODO: Send user to nick again due to error
			send(client_fd, "Invalid nickname\n", 17, 0);
			close(client_fd);
			//return (-1);
		}
		std::string cleaned_input_pwd = cleanInput(inputPwd);
		if(cleaned_input_pwd == _client.getUserPass(cleaned_input_nick))
		{
			_client.setUser(cleaned_input_nick, cleaned_input_pwd);
			_client.setAuthenticated(true);
			std::map<std::string, std::string>::iterator it = _client.getNick(cleaned_input_nick);
			std::string welcome = "Welcome to IRC server\n\nYour Data:\n" + it->first + "\n";
			send(client_fd, welcome.c_str(), welcome.length(), 0);
		}
		else
		{
			send(client_fd, "Invalid password\n", 17, 0);
			close(client_fd);
		}
	}
	else
	{
		_client.setUser(cleaned_input_nick, "");
		send(client_fd, "PASS:\n", 5, 0);
		std::string inputPwd = readLine(client_fd, 510); //(510) Max pwd characters
		if(inputPwd.empty())
		{
			//TODO: Send user to nick again due to error
			send(client_fd, "Invalid password\n", 17, 0);
			close(client_fd);
			//return (-1);
		}
		std::string cleaned_input_pwd = cleanInput(inputPwd);
		_client.setUser(cleaned_input_nick, cleaned_input_pwd);
		_client.setAuthenticated(true);
		std::map<std::string, std::string>::iterator it = _client.getNick(cleaned_input_nick);
		std::string welcome = "Welcome to IRC server\n\nYour Data:\n" + it->first + "\n";
		send(client_fd, welcome.c_str(), welcome.length(), 0);
	}
}

/**
 * @brief Command to change the nickname of the user
 * @param client_fd File descriptor of the client
*/
void Commands::nickCommand(int client_fd, std::string restOfCommand)
{
	std::string inputNick = restOfCommand;
	if(inputNick.empty())
	{
		send(client_fd, "Invalid nickname\n", 17, 0);
	}
	std::string cleaned_input_nick = cleanInput(inputNick);
	_client.setUser(cleaned_input_nick, _client.getUserPass(cleaned_input_nick));
}

/**
 * @brief Command to join a channel
 * @param client_fd File descriptor of the client
*/
void Commands::joinCommand(int client_fd, std::string restOfCommand)
{
	std::string inputChannel = restOfCommand;
	if(inputChannel.empty())
	{
		send(client_fd, "Invalid channel\n", 16, 0);
	}
	std::string cleaned_input_channel = cleanInput(inputChannel);
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
	std::string cleaned_input_channel = cleanInput(inputChannel);
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
		//TODO: List all channels
	}
	else
	{
		//TODO: List all channels with the full/part name of the channel
	}
}

void Commands::usersCommand(int client_fd)
{
	//TODO: List all users in the channel
}

void Commands::exitCommand(int client_fd)
{
	//TODO: Leave channel and close connection
}

void Commands::kickCommand(int client_fd, std::string restOfCommand)
{
	std::string inputNick = restOfCommand;
	if(inputNick.empty())
	{
		send(client_fd, "Invalid nickname\n", 17, 0);
	}
	std::string cleaned_input_nick = cleanInput(inputNick);
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
	std::string cleaned_input_nick = cleanInput(inputNick);
	//TODO: Invite user to channel
}

void Commands::topicCommand(int client_fd, std::string restOfCommand)
{
	std::string inputTopic = restOfCommand;
	if(inputTopic.empty())
	{
		send(client_fd, "Invalid topic\n", 14, 0);
	}
	std::string cleaned_input_topic = cleanInput(inputTopic);
	//TODO: Change topic of the channel
}

void Commands::modeCommand(int client_fd, std::string restOfCommand)
{
	std::string inputMode = restOfCommand;
	if(inputMode.empty())
	{
		send(client_fd, "Invalid mode\n", 13, 0);
	}
	std::string cleaned_input_mode = cleanInput(inputMode);
	//TODO: Change mode of the channel
}

void Commands::parseCommand(int client_fd, std::string command)
{

	std::string commands[10] = {"/nick", "/join", "/leave", "/list", "/users", "/exit", "/kick",  "/invite", "/topic", "/mode"};
	int i = 0;
    std::string foundCommand;
    std::string restOfCommand;

    for(i; i < 10; i++)
    {
        size_t pos = command.find(commands[i]);
        if(pos != std::string::npos)
        {
            foundCommand = commands[i];
            restOfCommand = command.substr(pos + commands[i].length());
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