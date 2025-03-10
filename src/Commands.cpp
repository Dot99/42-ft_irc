/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 12:19:16 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/10 12:58:16 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Commands.hpp"

Commands::Commands()
{
}

Commands::~Commands()
{
}

void Commands::parseCommand(std::string command, std::string param, int client_fd, Client *client)
{
	std::istringstream iss(buffer);
	std::string command, param;
	iss >> command >> param;
	if (command != "PASS")
	{
		send(client_fd, "ERROR :You must authenticate with PASS first\r\n", 46, 0);
		return;
	}
	else
	{
		if (_clients[client_fd].isAuthenticated)
		{
			send(client_fd, "ERROR: Already authenticated\r\n", 30, 0);
			return;
		}
		if (param == _server->getPwd())
		{
			_clients[client_fd].isAuthenticated = true;
			send(client_fd, "Password accepted. Please send NICK and USER.\r\n", 46, 0);
		}
		else
		{
			send(client_fd, "ERROR :Incorrect password\r\n", 27, 0);
			close(client_fd);
			removeClient(client_fd);
		}
	}		
}