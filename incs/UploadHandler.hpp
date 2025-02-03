#ifndef UPLOADHANDLER_HPP
#define UPLOADHANDLER_HPP

#include <string>
#include "HttpParser.hpp"
#include "ErrorHandler.hpp"

class UploadHandler {
    private:

    public:
        UploadHandler();
        ~UploadHandler();

        std::string upload(HttpParser &parser);
        std::string uploadReturnPath(HttpParser &parser);
};

#endif
