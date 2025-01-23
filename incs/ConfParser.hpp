#ifndef CONFPARSER_HPP
#define CONFPARSER_HPP

#include "Webserver.hpp"
#include <vector> 


class ConfParser {
private:
    std::string              _fileName;
    off_t                    _fileSize;
    int                      _serverBlocks;

    std::vector<std::string> _fileLines;
    std::vector<std::string> _fileData;         //for info that needs to store

public:
    ConfParser(std::string filename);
    ~ConfParser();
    // ConfParser(const ConfParser &other);
    // ConfParser& operator=(const ConfParser &other);


    //validate file basics before opening
    bool fileValidation();
    bool fileExists();
    bool filePermissions();
    bool fileExtension();

    //parse file
    bool        parseFile();
    std::string removeComments(const std::string &line);
    void        checkServerBlock(const std::string &line, int &block);

    //GETTERS
    std::vector<std::string> getListen();

    // Member functions
    void display();
};

#endif 
