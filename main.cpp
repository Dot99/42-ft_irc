/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 10:26:42 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/03 12:18:11 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcServer.hpp"
#include "utils.hpp"
#include <string>

int main(int argc, char **argv)
{
	std::string args[argc -1];
	if(checkArgs(argc, argv, args));
		return(1);
	try
	{
		IrcServer irc(args);
		irc.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}