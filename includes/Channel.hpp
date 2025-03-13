#pragma once
#include "Utils.hpp"	

class Client;
class Channel
{
    private:
        std::string _name;
		std::string _password;
		std::string _topic;
		std::string _mode;
		bool _inviteOnly;
		size_t _limit;
        std::vector<Client *> _users;
		std::vector<Client *> _operators;
		std::vector<Client *> _invitedUsers;
    public:
        Channel(std::string name);
        ~Channel();
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
		size_t getLimit() const;
		std::string getPassword() const;
		std::string getTopic() const;
		std::string getMode() const;
		bool getInviteOnly() const;
        std::string getName() const;
        std::vector<Client *> getUsers() const;
		Client *getInvitedUser(int fd);
		Client * getUserFd(int fd);
};