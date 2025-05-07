#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "Global.hpp"
#include "HttpRequest.hpp"
#include "CgiHandler.hpp"
#include "StaticHandler.hpp"
#include "Logger.hpp"

class Router {
private:
    Router();

public:
    static Router& getInstance();
    ~Router();

    std::string routeRequest(HttpRequest &req);
};

#endif

