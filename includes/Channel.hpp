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
        std::vector<Client *> _users;
		std::vector<Client *> _operators;
    public:
        Channel(std::string name);
        ~Channel();
        void addUser(Client *client);
        void removeUser(Client *client);
        std::string getName() const;
        std::vector<Client *> getUsers() const;
		Client * getUserFd(int fd);
};