#include "StaticHandler.hpp"

StaticHandler::StaticHandler() {};

StaticHandler::~StaticHandler() {};

/**
 * @brief   Calls for correct function to handle the static request being processed.
 */
std::string StaticHandler::processRequest(HttpRequest &req) {
    std::string uri = req.getUri();
    std::string method = req.getMethod();
    
    if (method == "POST") {
        if (req.getBody().empty()) {
            Logger::getInstance().logLevel("ERROR", "POST request has an empty body!", 500);
        } else {
            return Methods::mPost(req);
        }
    }
    if (method == "DELETE") {
        return Methods::mDelete(req);
    }
    return Methods::mGet(req);
}

