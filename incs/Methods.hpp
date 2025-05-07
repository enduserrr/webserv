#ifndef METHODS_HPP
# define METHODS_HPP

#include "Global.hpp"
#include "HttpRequest.hpp"
#include "UploadHandler.hpp"
#include "Logger.hpp"
#include "Types.hpp"

class Methods {
    public:
        Methods();
        ~Methods();

        static std::string mGet(HttpRequest &req);
        static std::string mPost(HttpRequest &req);
        static std::string mDelete(HttpRequest &req);
        static std::string generateDirectoryListing(const std::string &directoryPath, const std::string &uri);
};

#endif
