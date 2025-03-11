#include "IrcServer.hpp"

class Channel
{
    private:
        std::string _name;
        std::vector<Client *> _clients;
    public:
        Channel(std::string name);
        ~Channel();
        void addClient(Client *client);
        void removeClient(Client *client);
        std::string getName() const;
        std::vector<Client *> getClients() const;
};