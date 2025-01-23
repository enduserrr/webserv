#ifndef CONFPARSER_HPP
#define CONFPARSER_HPP

#include "Webserver.hpp"
#include "ServerBlock.hpp"
#include <vector> 
#include <sstream>

#define SERVER          "server {"
#define SERVER_NAME     "server_name"
#define PORT            "listen"
#define BODY_SIZE       "client_max_body_size"
#define ERROR_PAGE      ""
#define METHODS         "allow_methods"
#define LOCATION        "" 

class ConfParser {
private:
    std::string              _fileName;
    off_t                    _fileSize;
    int                      _serverBlocks;

    std::vector<std::string> _fileLines;
    std::vector<std::string> _fileData;         //for info that needs to store
    std::vector<ServerBlock> _servers; 


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
    bool        checkServerBlock(const std::string &line, int &block);
    bool        BracketsClosed(int block);

    //parse data
    bool        parseData();
    void        keyWordFinder(std::string line, int serverIndex);

    //GETTERS
    //std::vector<std::string> getListen();

    // Member functions
    void display();
};

#endif 


/*
 * Parsing features: 
 * 
 * Parameters: 
 * - fileName     --> name of the configuration file
 * - fileSize     --> for storing file size 
 * - serverBlocks --> used for storing amount of server blocks in conf file
 * - fileLines    --> stores valid lines from configuration file
 * - fileData     --> no use yet
 * 
 * 
 * 
 * Construction: 
 * - Takes file name as constructor parameter
 * - Sets other parameter to zero 
 * 
 * fileValidation:
 * - First validates the file with some checks: 
 *      - File has to exists 
 *      - Filesize has to be in range of FILE_MIN and FILE_MAX
 *      - File has correct permissions (read for now, but can be updated)
 *      - File has to end to .conf (only .conf accectable)
 * 
 * 
 * -- Now we know that file is ok and we can open it and start parsing -- 
 * 
 * 
 * parseFile: 
 * - Opens file
 * - Reads file, line by line: 
 *      - If line has "#" meaning, its a comment. --> remove comment
 *      - Counts amount of server blocks and stores it. 
 *      - If line is not empty --> add line to vector of strings
 *      - 
 *     
 * - usable lines are now stored and we can close file. 
 * - if curly brackets are not closed --> return false 
 * 
 *  
 *  -- Now we need to find usable information from lines -- 
 * 
 * 
 * 
 */