/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 12:19:16 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/12 10:31:42 by gude-jes         ###   ########.fr       */
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

void Commands::parseCommand(int client_fd, std::string command)
{
	(void)client_fd;	
	if(command == "/nick")
	{
		//TODO: Allow user to change to another nickname
	}
	else if (command == "/join")
	{
		//TODO: If channel exist:
		// Check if has password and prompt user to input it
		// If password is correct, join channel
		// Else, send error message
		// If channel does not exist:
		// Create channel
		// Join channel
		// Give operator to user
	}
}