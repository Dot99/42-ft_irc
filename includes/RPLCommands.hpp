/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPLCommands.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 14:50:27 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/20 14:28:51 by gude-jes         ###   ########.fr       */
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
#define RPL_ENDOFWHO(mask) ("315 " + mask + " :End of WHO list\n")

#define RPL_LIST "322"
#define RPL_LISTEND "323"

#define RPL_CHANNELMODEIS(channel, mode, params) ("324 " + channel + " " + mode + " " + params + "\n")

#define RPL_TOPIC(nick, channel, topic) (":" + SERVER_NAME + " 332 " + nick + " " + channel + " :" + topic + "\r\n")

#define RPL_TOPICWHOTIME "333"

#define RPL_WHOREPLY(nick, channel, user, host, realname) ("325" + nick + " " + channel + " " + user + " " + host + " " + SERVER_NAME + " " + realname + " H :0 " + realname + "\r\n")

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

#define ERR_NICKNAMEINUSE(nick) (": 433 " + nick + " :Nickname is already in use\r\n")

#define ERR_ERRONEUSNICKNAME(nick) "432" + nick + " :Erroneus nickname\n"

#define ERR_NICKCOLLISION(nick) ("436 " + nick + " :Nickname collision KILL\n")

#define ERR_NOTONCHANNEL(nick, channel) ("442 " + nick + " " + channel + " :You're not on that channel\n")

#define ERR_NEEDMOREPARAMS(command) ("461 " + command + " :Not enough parameters\n")

#define ERR_ALREADYREGISTRED "462 :You may not reregister\n"

#define ERR_PASSWDMISMATCH "464 :Password incorrect\n"

#define ERR_UNKNOWNMODE(mode) ("472 " + mode + " :is unknown mode char to me\n")

#define ERR_CHANNOPRIVSNEEDED(channel) ("482 " + channel + " :You're not channel operator\n")
/*-----------------------------*/

