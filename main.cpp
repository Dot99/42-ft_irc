/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 10:26:42 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/31 10:36:20 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

IrcServer *server = NULL;

void signalHandler(int signum)
{
	if (signum == SIGPIPE)
		return;
	if (server)
	{
		delete server;
		server = NULL;
	}
	exit(signum);
}

int main(int argc, char **argv)
{
	if (checkArgs(argc, argv))
		return(1);
	// try
	// {
		signal(SIGINT, signalHandler);
		signal(SIGQUIT, signalHandler);
		signal(SIGPIPE, signalHandler);
		server = new IrcServer(argv, argc);
		server->run();
	// }
	// catch(const std::exception& e)
	// {
	// 	std::cerr << e.what() << '\n';
	// 	return 1;
	// }
	return (0);
}