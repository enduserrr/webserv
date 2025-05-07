#ifndef CONFPARSER_HPP
#define CONFPARSER_HPP

#include "Global.hpp"
#include "ServerBlock.hpp"

class ConfParser {
private:
    std::string              _fileName;
    off_t                    _fileSize;
    int                      _serverBlocks;

    std::vector<std::string> _fileLines;
    std::vector<std::string> _fileData;
    std::vector<ServerBlock> _servers;

public:
    //Constructor & Destructor
    ConfParser(std::string filename);
    ~ConfParser();

    //GETTERS
    std::vector<ServerBlock>& getServers();

    //FILE VALIDATION
    void                    fileValidation(); //
    void                    fileExists();  //
    void                    filePermissions(); //
    void                    fileExtension(const std::string &path, const std::string &ext); //
    void                    fileSize(); //

    //FILE PARSING
    void                    parseFile();
    void                    blocks(int block);
    void                    parseLine(std::string &line, int &block, bool &hasContent);
    bool                    serverLine(std::string &line, int &block);

    //DATA PARSING
    void                    parseData();
    void                    keyWordFinder(std::string line, int serverIndex);
    void                    locationBlock(int serverIndex, size_t &i);
    void                    parseMethods(std::istringstream &ss, Location &loc);
    template <typename T>
    void                    parseSingle(std::istringstream &ss, T &obj, void (T::*setter)(const std::string&));
    template <typename T>
    void                    parseCodeValue(std::istringstream &ss, T &obj, void (T::*setter)(int, const std::string&));
    void                    allSetted();
    void                    locationFallbacks(int si, Location &loc);

};

//HELPERS
void                        whiteSpaceTrim(std::string &str);
std::string                 removeComments(const std::string &line);

#endif
