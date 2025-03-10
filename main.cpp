/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 10:26:42 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/10 10:25:24 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcServer.hpp"
#include "Client.hpp"
#include "Utils.hpp"
#include <string>

int main(int argc, char **argv)
{
	std::string args[argc -1];
	if (checkArgs(argc, argv, args))
	return(1);
	try
	{
		Client client(0);
		IrcServer irc(args, client);
		irc.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}