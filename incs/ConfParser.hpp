/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:45:22 by eleppala          #+#    #+#             */
/*   Updated: 2025/03/16 10:27:30 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef CONFPARSER_HPP
#define CONFPARSER_HPP

#include "Webserver.hpp"
#include "ServerBlock.hpp"
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>

//KEYWORDS
#define SERVER          "server {"
#define SERVER_NAME     "server_name"
#define PORT            "listen"
#define BODY_SIZE       "client_max_body_size"
#define ERR_PAGE        "error_page"
#define METHODS         "allow_methods"
#define LOCATION        "location"
#define ROOT            "root"
#define AUTOI           "autoindex"
#define INDEX           "index"
#define REDIR           "return"
#define USTORE          "upload_store"


//ERRORS
#define ERR RED         "error: " RES
#define FILE_V GRAY     "file validation: " RES
#define CONF GRAY       "configuration file: " RES

//FILESIZES
#define FILE_SIZE_MAX 1000000000
#define FILE_SIZE_MIN 0

class ConfParser {
private:
    std::string              _fileName;
    off_t                    _fileSize;
    int                      _serverBlocks;

    std::vector<std::string> _fileLines;
    std::vector<std::string> _fileData;         //for info that needs to store
    std::vector<ServerBlock> _servers;

public:
    //Constructor & Destructor
    ConfParser(std::string filename);
    ~ConfParser();

    //GETTERS
    std::vector<ServerBlock>& getServers();


    //FILE VALIDATION
    void                    fileValidation();
    void                    fileExists();
    void                    filePermissions();
    void                    fileExtension(const std::string &path, const std::string &ext);
    void                    fileSize();


    //FILE PARSING
    void                    parseFile();
    void                    blocks(int block);
    void                    parseLine(std::string &line, int &block);


    //DATA PARSING
    void                    parseData();
    void                    keyWordFinder(std::string line, int serverIndex);
    void                    locationBlock(int serverIndex, size_t &i);
    void                    parseMethods(std::istringstream &ss, Location &loc);
    template <typename T>
    void                    parseSingle(std::istringstream &ss, T &obj, void (T::*setter)(const std::string&));
    template <typename T>
    void                    parseCodeValue(std::istringstream &ss, T &obj, void (T::*setter)(int, const std::string&));


    // DEBUG
    void                    display();
};

//HELPERS
void                        whiteSpaceTrim(std::string &str);
std::string                 removeComments(const std::string &line);

#endif
