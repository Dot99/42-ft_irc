/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 10:26:42 by gude-jes          #+#    #+#             */
/*   Updated: 2025/04/08 09:20:15 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

IrcServer *server = NULL;

/**
 * @brief Check the arguments passed to the program.
 * 
 * @param signum The signal number.
*/
void signalHandler(int signum)
{
	if (signum == SIGPIPE || signum == SIGSEGV)
		return;
	if (server)
	{
		delete server;
		server = NULL;
	}
	exit(signum);
}

/**
 * @brief  Check the arguments passed to the progra and start the server.
 * 
 * @param argc Number of arguments.
 * @param argv Arguments passed to the program.
 * @return int 0 if success, 1 if error.
*/
int main(int argc, char **argv)
{
	if (checkArgs(argc, argv))
		return(1);
	try
	{
		signal(SIGINT, signalHandler);
		signal(SIGQUIT, signalHandler);
		signal(SIGPIPE, signalHandler);
		server = new IrcServer(argv, argc);
		server->run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		if (server)
			delete server;
		server = NULL;
		return 1;
	}
	return (0);
}