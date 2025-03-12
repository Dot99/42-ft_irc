/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 12:05:02 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/11 12:46:40 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

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
	for(int i = 0; i < argc; i++)
	{
		args[i] = argv[i];
		//std::cout << "args[" << i << "]: " << args[i] << std::endl;
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

/**
 * @brief Reads input from the client until a newline (`\n`) or reaches the max allowed length. Excess input is discarded to prevent buffer overflows.
 * @param client_fd File descriptor of the client
 * @param max_length Maximum length of the input
 */
std::string readLine(int client_fd, unsigned long max_length) {
    std::string input;
    char c;
    int bytes_received;
    bool too_long = false;

    while ((bytes_received = recv(client_fd, &c, 1, 0)) > 0) {
        if (c == '\n' || c == '\r')  // End of input
            break;
        if (!too_long) {
            if (input.size() < max_length) {
                input += c;
            } else {
                too_long = true;  // Mark that we are exceeding the limit
            }
        }
    }

    // Discard any remaining input if it was too long
    if (too_long) {
        char discard_buffer[256];
        while (recv(client_fd, discard_buffer, sizeof(discard_buffer), MSG_DONTWAIT) > 0);
        return "";  // Return empty string to indicate an error
    }

    return input;
}

void sendClientMsg(int client_fd, const char *msg, int flags) {
    std::time_t now = std::time(NULL);
    std::tm* localTime = std::localtime(&now);
    int hour = localTime->tm_hour;
	int min = localTime->tm_min;
	std::stringstream ss;
    ss << "[" << hour << ":" << min << "] " << msg;
	std::string full_msg = ss.str();
	if (send(client_fd, full_msg.c_str(), full_msg.length(), flags) == -1) {
		std::cerr << "Error: send() failed" << std::endl;
	}
}

std::string clean_input(std::string input)
{
	std::string result;
    for (std::string::iterator it = input.begin(); it != input.end(); ++it)
	{
        if (*it != '\n' && *it != '\r' && *it != ' ')
            result += *it;
    }
	return result;
}