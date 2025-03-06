/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 12:05:02 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/03 12:19:29 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

/**
 * @brief Check if a string is a number
 * 
 * @param str String to check
 * @return true If the string is a number
 * @return false If the string is not a number
*/
bool isNumber(std::string str)
{
	std::string::iterator it = str.begin();
	while (it != str.end() && std::isdigit(*it))
		it++;
	if(it == str.end() && !str.empty())
		return (true);
	return (false);
}

/**
 * @brief Check if the arguments are valid
 * 
 * @param argc Number of arguments
 * @param argv Arguments
 * @param args Array to store the arguments
 * @return true If the arguments are invalid
 * @return false If the arguments are valid
*/
bool checkArgs(int argc, char **argv, std::string args[])
{
	std::cout << "argc: " << argc << std::endl;
	for(int i = 0; i < argc; i++)
	{
		args[i] = argv[i];
		std::cout << "args[" << i << "]: " << args[i] << std::endl;
	}
	if(argc != 3)
	{
		std::cout << "Error: Invalid number of arguments" << std::endl;
		std::cout << "Usage: ./ircserv [port] [password]" << std::endl;
		return (true);
	}
	if(!isNumber(argv[1]))
	{
		std::cout << "Error: Invalid port" << std::endl;
		std::cout << "Usage: ./ircserv [port] [password]" << std::endl;
		return (true);
	}
	std::istringstream iss(args[1]);
	int port;
	if (!(iss >> port))
	{
		std::cerr << "Erro: Porta inválida\n";
		//std::exit(EXIT_FAILURE);
	}
	if(port < 0 || port > 65535)
	{
		std::cout << "Error: Invalid port(Between 0 and 65535)" << std::endl;
		std::cout << "Usage: ./ircserv [port] [password]" << std::endl;
		return (true);
	}
	if(args[1].size() == 0)
	{
		std::cout << "Error: Invalid password" << std::endl;
		std::cout << "Usage: ./ircserv [port] [password]" << std::endl;
		return (true);
	}
	return (false);
}
