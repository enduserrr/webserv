/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eleppala <eleppala@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:45:22 by eleppala          #+#    #+#             */
/*   Updated: 2025/01/28 18:45:24 by eleppala         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

#define SERVER          "server {"
#define SERVER_NAME     "server_name"
#define PORT            "listen"
#define BODY_SIZE       "client_max_body_size"
#define ERR_PAGE        "error_page"
#define METHODS         "allow_methods"
#define LOCATION        "location"
#define ROOT            "root"
#define AUTOI           "autoindex"

//ERRORS 
#define ERR RED         "error: " RESET
#define FILE_V GRAY     "file validation: " RESET
#define CONF GRAY       "configuration file: " RESET


//COLORS
#define RED             "\033[1;31m"
#define GRAY            "\033[0;90m"
#define RESET           "\033[0m"

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


    //validate file basics before opening
    void                    fileValidation();
    void                    fileExists();
    void                    filePermissions();
    void                    fileExtension(const std::string &path, const std::string &ext);
    void                    fileSize(); 

    //parse file
    void                    parseFile();
    std::string             removeComments(const std::string &line);
    void                    blocks(int block);
    void                    parseLine(std::string &line, int &block);

    //parse data
    void                    parseData();
    void                    keyWordFinder(std::string line, int serverIndex);
    void                    locationBlock(int serverIndex, size_t &i);
    void                    parseLocationLine(std::istringstream &ss, Location &loc);
    void                    parseRoot(std::istringstream &ss, std::string &root);
    void                    parseAutoIndex(std::istringstream &ss, bool &autoi);
    void                    parseMethods(std::istringstream &ss, Location &loc);
    void                    parseServerName(std::istringstream &ss, int si);
    void                    parsePort(std::istringstream &ss, int si);
    void                    parseBodySize(std::istringstream &ss, int si);
    void                    parseErrorPages(std::istringstream &ss, std::map<int, std::string> &error_pages);
    
    void                    whiteSpaceTrim(std::string &str);
    int                     convertToInt(std::string &word, const std::string &info);
    void                    hasForbiddenSymbols(std::string& word);
    bool                    hasValidUnit(std::string &word);
    void                    wordIsExpected(const std::string &word, const std::string &expected);

    // DEBUG
    void                    display();
};

#endif
