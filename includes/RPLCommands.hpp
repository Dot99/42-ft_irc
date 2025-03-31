/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPLCommands.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 14:50:27 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/28 10:07:24 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

/*------Connection replies-----*/
#define RPL_WELCOME(nick, network) ("001 " + nick + " :Welcome to "+ network + ", " + nick + "\n")

#define RPL_YOURHOST(server) ("002 :Your host is " + server + ", running version 1.0\n")

#define RPL_CREATED(date) ("003 :This server was created " + date + "\n")

#define RPL_MYINFO(server, nick, version) (server + " 004 " + nick + " " + version + " :Available user modes: io, channel modes: tkl\r\n")
/*-----------------------------*/


#define RPL_JOIN(nick, user,host, channel) (":" + nick + "!" + user + "@" + host + " JOIN " + channel + "\r\n")


/*-------Channel replies------ */
#define RPL_ENDOFWHO(nick, mask) (nick + " 315 " + mask + " :End of /WHO list\r\n")

#define RPL_LISTSTART "321 Channel :Users Name\r\n"
#define RPL_LIST(channel, clientcount, topic) ("322 " + channel + " " + clientcount + " :" + topic + "\r\n")
#define RPL_LISTEND "323\r\n"

#define RPL_CHANNELMODEIS(channel, mode, params) ("324 " + channel + " " + mode + " " + params + "\n")

#define RPL_NOTOPIC(channel) ("331 " + channel + " :No topic is set\n")

#define RPL_TOPIC(channel, topic) (": 332" + channel + " :" + topic + "\r\n")

#define RPL_TOPIC2(nickname, channel, topic) (": 332 " + nickname + " " + channel + " :" + topic + + "\r\n")

#define RPL_TOPICWHOTIME(channel, nick, time) (":" + SERVER_NAME + " 333 " + nick + " " + channel + " " + time + "\r\n")

#define RPL_INVITING(nick, channel) (": 341" + nick + " " + channel + "\r\n")

#define RPL_WHOREPLY(server, client, channel, user, host, nick, flags, realname) (server + " 352 " + client + " " + channel + " " + user + " " + host + " " + server + " " + nick + " " + flags + " :0 " + realname + "\r\n")

#define RPL_NAMREPLY(nick, channel, users) (": 353 " + nick + " = " + channel + " :" + users + "\r\n")

#define RPL_ENDOFNAMES(channel) (": 366 " + channel + " :End of /NAMES list\r\n")
/*-----------------------------*/


/*-------Miscellaneous---------*/
#define RPL_MOTDSTART(nick) ("375 " + nick + " :- " + SERVER_NAME + " Message of the day - \r\n" )
#define RPL_MOTD(nick, line) ("372 " + nick + ":- " + line + "\r\n")
#define RPL_ENDOFMOTD(nick) "376 " + nick + ":End of /MOTD command.\r\n"

/*-----------------------------*/


/*---------Error replies-------*/
#define ERR_NOSUCHNICK(nick) ("401 " + nick + " :No such nick/channel\n")

#define ERR_NOSUCHCHANNEL(channel) ("403 " + channel + " :No such channel\n")

#define ERR_UNKNOWNCOMMAND(command) ("421 " + command + " :Unknown command\n")

#define ERR_NONICKNAMEGIVEN "431 :No nickname given\n"

#define ERR_NICKNAMEINUSE(nick) (": 433 * " + nick + " :Nickname is already in use\r\n")

#define ERR_ERRONEUSNICKNAME(nick) "432" + nick + " :Erroneus nickname\n"

#define ERR_NICKCOLLISION(nick) ("436 " + nick + " :Nickname collision KILL\n")

#define ERR_NOTONCHANNEL(nick, channel) ("442 " + nick + " " + channel + " :You're not on that channel\n")

#define ERR_USERONCHANNEL(nick, channel) ("443 " + nick + " " + channel + " :is already on channel\n")

#define ERR_NOTREGISTERED "451 :You have not registered\n"

#define ERR_NEEDMOREPARAMS(command) ("461 " + command + " :Not enough parameters\n")

#define ERR_ALREADYREGISTERED "462 :You may not reregister\n"

#define ERR_PASSWDMISMATCH "464 :Password incorrect\n"

#define ERR_CHANNELISFULL(channel) ("471 " + channel + " :Cannot join channel (+l)\n")

#define ERR_UNKNOWNMODE(mode) ("472 " + mode + " :is unknown mode char to me\n")

#define ERR_INVITEONLYCHAN(channel) ("473 " + channel + " :Cannot join channel (+i)\r\n")

#define ERR_CHANOPRIVSNEEDED(channel) ("482 " + channel + " :You're not channel operator\n")
/*-----------------------------*/

