/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPLCommands.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 14:50:27 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/18 16:26:59 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

/*------Connection replies-----*/
#define RPL_WELCOME(nick, network) ("001 " + nick + " :Welcome to "+ network + ", " + nick + "\n")

#define RPL_YOURHOST(server) ("002 :Your host is " + server + ", running version 1.0\n")

#define RPL_CREATED(date) ("003 :This server was created " + date + "\n")

#define RPL_MYINFO(server, version) ("004 " + server + " " + version + " :Available user modes: io, channel modes: tkl\n")
/*-----------------------------*/


/*-------Channel replies------ */
#define RPL_LIST "322"
#define RPL_LISTEND "323"

#define RPL_TOPIC(nick, channel, topic) (":" + SERVER_NAME + " 332 " + nick + " " + channel + " :" + topic + "\r\n")

#define RPL_TOPICWHOTIME "333"

#define RPL_NAMREPLY(nick, channel, users) (": 353 " + nick + " = " + channel + " :" + users + "\n")

#define RPL_ENDOFNAMES(nickname, channel) (": 366 " + nickname + " " + channel + " :End of NAMES list" + "\r\n")
/*-----------------------------*/


/*-------Miscellaneous---------*/
#define RPL_MOTDSTART "375"
#define RPL_MOTD "372"
#define RPL_ENDOFMOTD "376"
/*-----------------------------*/


/*---------Error replies-------*/
#define ERR_NOSUCHNICK(nick) ("401 " + nick + " :No such nick/channel\n")

#define ERR_NOSUCHCHANNEL(channel) ("403 " + channel + " :No such channel\n")

#define ERR_UNKNOWNCOMMAND(command) ("421 " + command + " :Unknown command\n")

#define ERR_NONICKNAMEGIVEN "431 :No nickname given\n"

#define ERR_NICKNAMEINUSE(nick) (nick + " :Nickname is already in use\n")

#define ERR_ERRONEUSNICKNAME(nick) "432" + nick + " :Erroneus nickname\n"

#define ERR_NEEDMOREPARAMS(command) ("461 " + command + " :Not enough parameters\n")

#define ERR_ALREADYREGISTRED "462 :You may not reregister\n"

#define ERR_PASSWDMISMATCH "464 :Password incorrect\n"
/*-----------------------------*/

