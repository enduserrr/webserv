#ifndef STATICHANDLER_HPP
#define STATICHANDLER_HPP

#include "Global.hpp"
#include "HttpRequest.hpp"
#include "Methods.hpp"

class StaticHandler {
    public:
        StaticHandler();
        ~StaticHandler();

        std::string processRequest(HttpRequest &req);
};

#endif
