/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 10:26:42 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/10 11:13:04 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcServer.hpp"
#include "Client.hpp"
#include "Utils.hpp"
#include <string>

int main(int argc, char **argv)
{
	std::string args[argc];
	if (checkArgs(argc, argv, args))
		return(1);
	try
	{
		IrcServer* server = new IrcServer(args, NULL);
		Client client(server);
		server->setClient(&client);
		server->run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}