/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 12:05:02 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/26 09:52:41 by gude-jes         ###   ########.fr       */
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
bool checkArgs(int argc, char **argv, std::vector<std::string> &args)
{
	args.clear();
	for(int i = 0; i < argc; i++)
		args.push_back(argv[i]);
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
	if(args[2].size() == 0)
	{
		std::cout << "Error: Invalid password" << std::endl;
		std::cout << "Usage: ./ircserv [port] [password]" << std::endl;
		return (true);
	}
	return (false);
}

/**
 * @brief Sanitizes the input by removing invalid control char
 * 
 * @param input Raw input string
 * @return std::string Sanitized input string with control characters removed
*/
std::string sanitizeInput(std::string input) {
	std::string sanitized;
	for (size_t i = 0; i < input.size(); i++) {
		if (isprint(input[i]) || input[i] == '\t' || input[i] == '\n' || input[i] == '\r') {
			sanitized += input[i];
		}
	}
	return sanitized;
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
		//Handle CLRF
		if (c == '\r') {
			// Peek the next character to check for '\n'
			char next_char;
			int peeked_bytes = recv(client_fd, &next_char, 1, MSG_PEEK);
			if (peeked_bytes > 0 && next_char == '\n') {
				recv(client_fd, &next_char, 1, 0);
			}
			break;
		}
		else if (c == '\n') {
			break;
		}
		if(iscntrl(c) && c != '\t')
			continue;
		if (!too_long) {
			if (input.size() < max_length )
				input += c;
			else 
				too_long = true;  // Mark that we are exceeding the limit
		}
	}

	// Discard any remaining input if it was too long
	if (too_long) {
		char discard_buffer[256];
		while (recv(client_fd, discard_buffer, sizeof(discard_buffer), MSG_DONTWAIT) > 0);
		return "";  // Return empty string to indicate an error
	}

	return sanitizeInput(input);
}

void sendClientMsg(int client_fd, std::string msg)
{
	if (send(client_fd, msg.c_str(), msg.length(), 0) == -1)
	{
		std::cerr << "Error sending response" << std::endl;
		throw std::exception();
	}
	std::cout << "Sent to client[" << client_fd << "]: " << msg;
}

std::string clean_input(std::string input, int what )
{
	std::string result;
	for (std::string::iterator it = input.begin(); it != input.end(); ++it)
	{
		if (what == SPACES)
			if (*it != ' ')
				result += *it;
		if (what == ENTER)
	   		if (*it != '\n' && *it != '\r')
				result += *it;
	}
	return result;
}

bool wildcardMatch(const std::string &str, const std::string &pattern)
{
	size_t strIndex = 0, patternIndex = 0, matchIndex = 0;
	size_t starIndex = std::string::npos;
	while (strIndex < str.size())
	{
		if (patternIndex < pattern.size() && (pattern[patternIndex] == '?' || pattern[patternIndex] == str[strIndex]))
		{
			strIndex++;
			patternIndex++;
		}
		else if (patternIndex < pattern.size() && pattern[patternIndex] == '*')
		{
			starIndex = patternIndex;
			matchIndex = strIndex;
			patternIndex++;
		}
		else if (starIndex != std::string::npos)
		{
			patternIndex = starIndex + 1;
			matchIndex++;
			strIndex = matchIndex;
		}
		else
		{
			return false;
		}
	}
	while (patternIndex < pattern.size() && pattern[patternIndex] == '*')
	{
		patternIndex++;
	}
	return patternIndex == pattern.size();
}

std::string checkNick(const std::string& nick, std::vector<Client *> &users)
{
	if (nick.empty())
		return ERR_NONICKNAMEGIVEN;
	if (isdigit(nick[0]))
		return ERR_ERRONEUSNICKNAME(nick);
	
	for (size_t i = 0; i < nick.length(); i++)
	{
		char c = nick[i];
		if (!isalnum(c) && c != '[' && c != ']' && c != '{' && c != '}' && c != '\\' && c != '|' && c != '_')
			return ERR_ERRONEUSNICKNAME(nick);
	}

	// Convert input nick to lowercase
	std::string lowercaseNick = nick;
	std::transform(lowercaseNick.begin(), lowercaseNick.end(), lowercaseNick.begin(), ::tolower);

	for (size_t i = 0; i < users.size(); i++)
	{
		std::string existingNick = users[i]->getNick();
		std::transform(existingNick.begin(), existingNick.end(), existingNick.begin(), ::tolower);

		if (existingNick == lowercaseNick)
			return ERR_NICKNAMEINUSE(nick);
	}

	return "";
}
