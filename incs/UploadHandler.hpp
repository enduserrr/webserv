#ifndef UPLOADHANDLER_HPP
#define UPLOADHANDLER_HPP

#include "Global.hpp"
#include "HttpRequest.hpp"
#include "Logger.hpp"

class UploadHandler {
    public:
        UploadHandler();
        ~UploadHandler();

        std::string uploadReturnPath(HttpRequest &req);
};

#endif
