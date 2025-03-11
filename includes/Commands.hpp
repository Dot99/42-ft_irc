/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 12:18:45 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/11 11:47:45 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Client.hpp"
#include "IrcServer.hpp"
#include "Utils.hpp"

class Client;
class IrcServer;
class Commands
{
	private:
		IrcServer &_server;
		Client &_client;
	public:
		Commands(IrcServer &server, Client &client);
		~Commands();
		void parseCommand(int client_fd);
};