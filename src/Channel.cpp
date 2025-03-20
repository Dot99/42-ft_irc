/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 16:28:29 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/20 08:27:52 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

/**
 * @brief Construct a new Channel object
 * 
 * @param name Channel name
 */
Channel::Channel(std::string name)
{
    _name = name;
}

/**
 * @brief Destroy the Channel object
 * 
 */
Channel::~Channel()
{
}

/**
 * @brief Add a user to the channel
 * 
 * @param client Client
 */
void Channel::addUser(Client *client)
{
    _users.push_back(client);
}

/**
 * @brief Add an operator to the channel
 * 
 * @param client Client
 */
void Channel::addOperator(Client *client)
{
	_operators.push_back(client);
}

/**
 * @brief Add an invited user to the channel
 * 
 * @param client Client
 */
void Channel::addInvitedUser(Client *client)
{
	_invitedUsers.push_back(client);
}

/**
 * @brief Remove an invited user from the channel
 * 
 * @param client Client
 */
void Channel::removeInvitedUser(Client *client)
{
	for (size_t i = 0; i < _invitedUsers.size(); i++)
	{
		if (_invitedUsers[i] == client)
		{
			_invitedUsers.erase(_invitedUsers.begin() + i);
			break;
		}
	}
}

/**
 * @brief Remove an operator from the channel
 * 
 * @param client Client
 */
void Channel::removeOperator(Client *client)
{
	for (size_t i = 0; i < _operators.size(); i++)
	{
		if (_operators[i] == client)
		{
			_operators.erase(_operators.begin() + i);
			break;
		}
	}
}

/**
 * @brief Remove a user from the channel
 * 
 * @param client Client
 */
void Channel::removeUser(Client *client)
{
	for (size_t i = 0; i < _users.size(); i++)
	{
		if (_users[i] == client)
		{
			_users.erase(_users.begin() + i);
			break;
		}
	}
}

/**
 * @brief Set the channel name
 * 
 * @param name Channel name
 */
void Channel::setInviteOnly(bool inviteOnly)
{
	_inviteOnly = inviteOnly;
}

/**
 * @brief Set the channel name
 * 
 * @param name Channel name
 */
void Channel::setMode(std::string mode)
{
	_mode = mode;
}

/**
 * @brief Set the channel name
 * 
 * @param name Channel name
 */
void Channel::setPassword(std::string password)
{
	_password = password;
}

/**
 * @brief Set the channel name
 * 
 * @param name Channel name
 */
void Channel::setTopic(std::string topic)
{
	_topic = topic;
}

/**
 * @brief Set the channel name
 * 
 * @param name Channel name
 */
void Channel::setLimit(int limit)
{
	_limit = limit;
}

/**
 * @brief Get the Invited User object
 * 
 * @param fd File descriptor
 * @return Client* User
 */
Client *Channel::getInvitedUser(int fd)
{
	for (size_t i = 0; i < _invitedUsers.size(); i++)
	{
		if (_invitedUsers[i]->getFd() == fd)
		{
			return _invitedUsers[i];
		}
	}
	return NULL;
}

/**
 * @brief Get the limit of users in the channel
 * 
 * @return size_t Limit
*/
size_t Channel::getLimit() const
{
	return _limit;
}

/**
 * @brief Get the mode of the channel
 * 
 * @return std::string Mode
*/
std::string Channel::getMode() const
{
	return _mode;
}

/**
 * @brief Get the password of the channel
 * 
 * @return std::string Password
*/
std::string Channel::getPassword() const
{
	return _password;
}

/**
 * @brief Get the topic of the channel
 * 
 * @return std::string Topic
*/
std::string Channel::getTopic() const
{
	return _topic;
}

/**
 * @brief Get the invite only of the channel
 * 
 * @return bool Invite only
*/
bool Channel::getInviteOnly() const
{
	return _inviteOnly;
}

/**
 * @brief Get the name of the channel
 * 
 * @return std::string Name
*/
std::string Channel::getName() const
{
    return _name;
}

/**
 * @brief Get the users in the channel
 * 
 * @return std::vector<Client *> Users
*/
std::vector<Client *> Channel::getUsers() const
{
    return _users;
}

/**
 * @brief Get the User Fd object
 * 
 * @param fd File descriptor
 * @return std::vector<Client *> Users
*/
Client *Channel::getUserFd(int fd)
{
    for (size_t i = 0; i < _users.size(); i++)
    {
        if (_users[i]->getFd() == fd)
        {
            return _users[i];
        }
    }
    return NULL;
}
