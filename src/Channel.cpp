/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 16:28:29 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/28 12:34:23 by gude-jes         ###   ########.fr       */
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
	_inviteOnly = false;
	_topicProtection = false;
	_limit = 0;
}

/**
 * @brief Destroy the Channel object
 * 
 */
Channel::~Channel()
{
	for (size_t i = 0; i < _users.size(); i++)
	{
		if(_users[i])
		{
			removeUser(_users[i]);
			_users[i] = NULL;
		}
	}
	_users.clear();
	for (size_t i = 0; i < _operators.size(); i++)
	{
		if(_operators[i])
		{
			delete _operators[i];
			_operators[i] = NULL;
		}
	}
	_operators.clear();
	for (size_t i = 0; i < _invitedUsers.size(); i++)
	{
		if(_invitedUsers[i])
		{
			delete _invitedUsers[i];
			_invitedUsers[i] = NULL;
		}
	}
	_invitedUsers.clear();
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
 * @brief Set the channel topic protection
 * 
 * @param topicProtection Topic protection
 */
void Channel::setTopicProtection(bool topicProtection)
{
	_topicProtection = topicProtection;
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
 * @brief Set the topic time of the channel
 * 
 * @param topic_time Topic time
*/
void Channel::setTopicTime(std::string topic_time)
{
	_topic_time = topic_time;
}	

/**
 * @brief Get the topic time of the channel
 * 
 * @return std::string Topic time
*/
std::string Channel::getTopicTime() const
{
	return _topic_time;
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
 * @brief Get the topic protection of the channel
 * 
 * @return bool Topic protection
*/
bool Channel::getTopicProtection() const
{
	return _topicProtection;
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