#ifndef UPLOADHANDLER_HPP
#define UPLOADHANDLER_HPP

#include <string>
#include "HttpRequest.hpp"
#include "ErrorHandler.hpp"

class UploadHandler {
    private:

    public:
        UploadHandler();
        ~UploadHandler();

        std::string upload(HttpRequest &req);
        std::string uploadReturnPath(HttpRequest &req);
};

#endif
