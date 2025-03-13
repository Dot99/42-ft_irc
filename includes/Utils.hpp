/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/07 08:57:34 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/11 12:46:31 by gude-jes         ###   ########.fr       */
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

#include "Client.hpp"
#include "Channel.hpp"
#include "IrcServer.hpp"

#define SPACES 1
#define ENTER 2

bool checkArgs(int argc, char **argv, std::string args[]);
std::string readLine(int client_fd, unsigned long max_length);
void sendClientMsg(int client_fd, const char *msg, int flags);
std::string clean_input(std::string input, int what );