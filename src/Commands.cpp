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

Commands::Commands(IrcServer &server, Client &client): _server(server), _client(client)
{
}

Commands::~Commands()
{
}

void Commands::parseCommand(std::string command, std::string param, int client_fd, char buffer[1024])
{
	std::istringstream iss(buffer);
	iss >> command >> param;
	char bufffer[1024];
	int bytes_received = recv(client_fd, bufffer, sizeof(bufffer) - 1, 0);
	if (bytes_received <= 0) // Client disconnected
	{
		std::cout << "Client " << client_fd << " disconnected." << std::endl;
		close(client_fd);
		// close(_server.getSock());
		return;
	}
	bufffer[bytes_received] = '\0';
	std::string input(bufffer);
	std::string cleaned_input = cleanInput(input);
	if (cleaned_input == _server.getPwd())
	{
		_client.setAuthenticated(true);
		send(client_fd, "NICK:\n", 5, 0);
		bytes_received = recv(client_fd, buffer, sizeof(bufffer) - 1, 0);
		if (bytes_received <= 0) // Client disconnected
		{
			std::cout << "Client " << client_fd << " disconnected." << std::endl;
			close(client_fd);
			// close(_server.getSock());
			return;
		}
		buffer[bytes_received] = '\0';
		_client.setNick(buffer);
		send(client_fd, "USER:\n", 5, 0);
		bytes_received = recv(client_fd, buffer, sizeof(bufffer) - 1, 0);
		if (bytes_received <= 0) // Client disconnected
		{
			std::cout << "Client " << client_fd << " disconnected." << std::endl;
			close(client_fd);
			// close(_server.getSock());
			return;
		}
		buffer[bytes_received] = '\0';
		_client.setUser(buffer);
		std::string welcome = "Welcome to IRC server\n\nYour Data\nNick:" +
		_client.getUser() + "User:" + _client.getNick() + "\n";
		send(client_fd, welcome.c_str(), welcome.length(), 0);
	}
	else
	{
		send(client_fd, "ERROR :Incorrect password\r\n\n", 27, 0);
		close(client_fd);
		_client.removeClient(client_fd);
	}		
}