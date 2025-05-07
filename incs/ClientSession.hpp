#ifndef CLIENTSESSION_HPP
#define CLIENTSESSION_HPP

#include "Global.hpp"
#include "HttpRequest.hpp"
#include "ServerBlock.hpp"
#include "Logger.hpp"


class ClientSession {
    private:
        size_t _requestCount;
        time_t _windowStartTime;
        std::string _serverName;
        static const time_t WINDOW_DURATION = 1;
        static const size_t MAX_REQUESTS_PER_WINDOW = 10;

    public:
        int fd;
        HttpRequest request;
        std::string buffer;
        ServerBlock _block;

        ClientSession();
        ClientSession(int clientFd);
        ~ClientSession();

    void        removeClient();
    bool        requestLimiter();
    void        setServerName(std::string name);
    std::string getServerName();
};

#endif
