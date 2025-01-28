#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include <iostream>
#include <map>

class HttpParser {
private:
    size_t                              _maxBodySize; //get from ServerBlock
    std::string                         _method; 
    std::string                         _uri;
    std::string                         _httpVersion; 
    std::map <std::string, std::string> _headers;       //key + value
    std::string                         _body; 

public:
    HttpParser();
    ~HttpParser();
    // HttpParser(const HttpParser &other);
    // HttpParser& operator=(const HttpParser &other);

    // // Getters and Setters
    // int getValue() const;
    // void setValue(int value);

    // // Member functions

    bool parseRequest(std::string& req, size_t max);
    bool parseStartLine(std::string& line);
    bool parseHeaders(std::string& line);
    bool parseBody(std::string& line);
    void whiteSpaceTrim(std::string& str);
    void display() const;
};

#endif 
