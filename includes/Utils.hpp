/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/07 08:57:34 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/18 16:38:42 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <unistd.h>
#include <string>
#include <poll.h>
#include <map>
#include <sstream>
#include <vector>
#include <signal.h>
#include <functional>
#include <csignal>

#include "Client.hpp"
#include "Channel.hpp"
#include "IrcServer.hpp"
#include "RPLCommands.hpp"

#define SPACES 1
#define ENTER 2

bool checkArgs(int argc, char **argv, std::string args[]);
std::string readLine(int client_fd, unsigned long max_length);
void sendClientMsg(int client_fd, std::string msg);
std::string clean_input(std::string input, int what );
bool wildcardMatch(const std::string &str, const std::string &pattern);