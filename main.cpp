/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 10:26:42 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/28 09:47:30 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

IrcServer *server = NULL;
std::vector<std::string> args;

void signalHandler(int signum)
{
	if (server)
	{
		delete server;
		std::vector<std::string>().swap(args);
	}
	exit(signum);
}

int main(int argc, char **argv)
{
	if (checkArgs(argc, argv, args))
		return(1);
	try
	{
		signal(SIGINT, signalHandler);
		signal(SIGQUIT, signalHandler);
		server = new IrcServer(args);
		server->run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	std::vector<std::string>().swap(args);
	return (0);
}