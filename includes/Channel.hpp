#pragma once
#include "Utils.hpp"


/**
 * @brief Channel class represents a channel in the IRC server.
 * It contains information about the channel name, password, topic, mode,
 * topic protection, invite only status, user limit, and the users in the channel.
 * It also provides methods to add and remove users, set and get channel properties,
 * and check if a user is an operator.
 * @details
 * - The class contains private member variables to store the channel properties.
 * - The constructor initializes the channel name and sets default values for other properties.
 * - The destructor is empty as there are no dynamic allocations.
 * - The class provides methods to add and remove users, set and get channel properties,
 *  and check if a user is an operator.
 * - The class uses a vector to store the users, operators, and invited users.
 * - The class uses a string to store the channel name, password, topic, mode, and topic time.
 * - The class uses a boolean to store the invite only status and topic protection status.
 * - The class uses a size_t to store the user limit.
 * - The class uses a pointer to the Client class to represent the users and operators.
 * - The class uses a pointer to the Client class to represent the invited users.
 * - The class uses a pointer to the Client class to represent the users in the channel.
 * 
*/
class Channel
{
    private:
        std::string _name;
		std::string _password;
		std::string _topic;
		std::string _mode;
		std::string _topic_time;
		bool _inviteOnly;
		bool _topicProtection;
		size_t _limit;
        std::vector<Client *> _users;
		std::vector<Client *> _operators;
		std::vector<Client *> _invitedUsers;
    public:
        Channel(std::string name);
        ~Channel();
		int isOperator(int client_fd);
        void addUser(Client *client);
		void addOperator(Client *client);
		void addInvitedUser(Client *client);
		void removeInvitedUser(Client *client);
		void removeOperator(Client *client);
        void removeUser(Client *client);
		void setPassword(std::string password);
		void setTopic(std::string topic);
		void setMode(std::string mode);
		void setInviteOnly(bool inviteOnly);
		void setLimit(int limit);
		void setTopicProtection(bool topicProtection);
		void setTopicTime(std::string topic_time);
		size_t getLimit() const;
		std::string getPassword() const;
		std::string getTopic() const;
		std::string getMode() const;
		bool getTopicProtection() const;
		std::string getTopicTime() const;
		bool getInviteOnly() const;
        std::string getName() const;
        std::vector<Client *> getUsers() const;
		Client *getInvitedUser(int fd);
		Client * getUserFd(int fd);
};