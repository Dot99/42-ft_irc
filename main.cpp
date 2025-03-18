/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 10:26:42 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/18 16:35:27 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"
IrcServer *server = NULL;

void signalHandler(int signum)
{
	if (server)
	{
		delete server;
	}
	exit(signum);
}

int main(int argc, char **argv)
{
	//TODO:MEMORY LEAKS AND USERNAME
	std::string args[argc];
	if (checkArgs(argc, argv, args))
		return(1);
	try
	{
		signal(SIGINT, signalHandler);
		server = new IrcServer(args);
		server->run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}