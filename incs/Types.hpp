#ifndef TYPES_HPP
# define TYPES_HPP

#include "Global.hpp"

class Types {
    private:
        Types(); //Singleton
        std::map<std::string, std::string>  _mimeTypes;
        std::vector<std::string>            _contentTypes;
    public:
        static Types& getInstance();
        ~Types();

        std::string getMimeType(const std::string &extension) const;
        bool isValidMime(const std::string &extension) const;
        bool isValidContent(const std::string &contentType) const;

};

#endif
