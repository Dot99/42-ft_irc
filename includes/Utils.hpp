/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/07 08:57:34 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/31 10:31:56 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
#include <algorithm>
#include <signal.h>
#include <functional>
#include <csignal>

#include "Client.hpp"
#include "Channel.hpp"
#include "IrcServer.hpp"
#include "RPLCommands.hpp"

#define SPACES 1
#define ENTER 2
#define USERLEN 18

class Channel;
class Client;
class IrcServer;

bool checkArgs(int argc, char **argv);
std::string readLine(int client_fd, unsigned long max_length);
void sendClientMsg(int client_fd, std::string msg);
std::string clean_input(std::string input, int what );
bool wildcardMatch(const std::string &str, const std::string &pattern);
std::string checkNick(const std::string& nick, std::vector<Client *> &users);
Channel *checkChannelName(const std::string& channelName, std::vector<Channel *> &channels);
std::string to_string(int num);
void sendMotd(int clientFd, const std::string& nick);