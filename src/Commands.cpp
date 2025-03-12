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
	(void)_client;
}

Commands::~Commands()
{
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