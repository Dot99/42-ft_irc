/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 10:34:13 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/03 12:21:54 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <string>

class IrcServer
{
	private:
		int _port;
		std::string _pwd;
		int _socket;
		struct sockaddr_in _addr;
	public:
		IrcServer();
		IrcServer(const std::string &port, const std::string &pwd);
		IrcServer &operator=(const IrcServer &rhs);
		~IrcServer();
		void startServer();
		void run();
};