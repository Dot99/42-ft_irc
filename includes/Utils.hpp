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

#include "IrcServer.hpp"
#include <ctime>
#include <sstream>

bool checkArgs(int argc, char **argv, std::string args[]);
std::string readLine(int client_fd, unsigned long max_length);
void sendClientMsg(int client_fd, const char *msg, int flags);

std::string cleanInput(std::string input);