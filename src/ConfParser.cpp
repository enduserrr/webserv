#include "ConfParser.hpp"

// Constructor
ConfParser::ConfParser(std::string filename) : _fileName(filename) {
    _fileSize = 0;
    _serverBlocks = 0;
}

// Destructor
ConfParser::~ConfParser() {}

// // Copy Constructor
// ConfParser::ConfParser(const ConfParser &other) {
//     *this = other;
// }

// // Copy Assignment Operator
// ConfParser& ConfParser::operator=(const ConfParser &other) {
//     if (this != &other) {
//         this->value = other.value;
//     }
//     return *this;
// }


// Basic checks 
bool ConfParser::fileValidation(){
    if (!fileExists())
        std::cout << "File: " << _fileName << " does not exists" << std::endl;
    else if (_fileSize > FILE_SIZE_MAX)
        std::cout << "File: " << _fileName << " is too big" << std::endl;
    else if (_fileSize <= FILE_SIZE_MIN)
        std::cout << "File: " << _fileName << " is empty" << std::endl;
    else if (!filePermissions())
        std::cout << "File: " << _fileName << " deficient permissions" << std::endl;
    else if (!fileExtension())
        std::cout << "File: " << _fileName << " does not end .conf" << std::endl;
    else
    {
        std::cout << "File validation OK!" << std::endl;
        return true;
    }
    return false;
}

bool ConfParser::fileExists(){
    struct stat buffer;
    if (stat(_fileName.c_str(), &buffer) == 0) {
        _fileSize = buffer.st_size;                  //for handling filesize
        return true;
    }
    return false; 
}


bool ConfParser::filePermissions(){
    return (access(_fileName.c_str(), R_OK) == 0); // read permissions only for now.. lets see if more needed
}


bool ConfParser::fileExtension(){
    return _fileName.size() >= 5 && _fileName.substr(_fileName.size() - 5) == ".conf";
}

// Parsing the file

bool ConfParser::parseFile(){
    std::ifstream file;       
    
    // Open file
    file.open(_fileName.c_str());
    if (!file.is_open()){
        std::cerr << "Failed to open file: " << _fileName << std::endl;
        return false;
    }
    // Rest of the parsing here
    std::string line;
    int         block = 0;
    while (std::getline(file, line)) {
        line = removeComments(line);
        checkServerBlock(line, block);
        if (!line.empty() && line.find_first_not_of(" \t") != std::string::npos) {      
            _fileLines.push_back(line);
        }
    }
    file.close();
    return true;
}

std::string ConfParser::removeComments(const std::string &line){
    size_t position = line.find("#");
    if (position != std::string::npos){
        return line.substr(0, position);
    }
    return line;
}

void ConfParser::checkServerBlock(const std::string &line, int &block) {
    if(block == 0 && line.find("server") != std::string::npos) {
        _serverBlocks ++;
    }
    if(line.find("{") != std::string::npos) {
        block ++;
    }
    if(line.find("}") != std::string::npos) {
        block --;
    }
}

// Debug
void ConfParser::display(){

    std::cout << "\n\nParser information\n" << std::endl;
    std::cout << "File name: " <<_fileName << std::endl;
    std::cout << "File size: "<<_fileSize << std::endl;
    std::cout << "Server blocks: "<<_serverBlocks << std::endl;
    std::cout << "\nParsed file now: " << std::endl;
    for (size_t i = 0; i < _fileLines.size(); ++i) {
        std::cout << _fileLines[i] << std::endl;
    }
}

