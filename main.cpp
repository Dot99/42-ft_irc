/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 10:26:42 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/11 09:47:03 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcServer.hpp"
#include "Client.hpp"
#include "Commands.hpp"
#include "Utils.hpp"
#include <string>

int main(int argc, char **argv)
{
	std::string args[argc];
	if (checkArgs(argc, argv, args))
		return(1);
	try
	{
		IrcServer server(args);
		Client client(server);
		Commands commands(server, client);
		server.run(client, commands);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}