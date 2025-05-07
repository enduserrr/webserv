#ifndef SERVERBLOCK_HPP
#define SERVERBLOCK_HPP

#include "Global.hpp"
#include "Logger.hpp"
#include "Location.hpp"

class ServerBlock {
private:
    std::string                     _serverName;
    std::string                     _host; 
    std::string                     _root;
    bool                            _autoIndex;
    size_t                          _bodySize;
    std::vector<int>                _ports;
    std::map<std::string, Location> _locations;
    std::map<int, std::string>      _errorPages;


public:
    //Constructor & destructor
    ServerBlock();
    ~ServerBlock();

    // Getters
    std::string                      getServerName() const;
    std::string&                     getRoot();
    std::vector<int>                 getPorts();
    bool                             getAutoIndex(const std::string &key);
    size_t                           getBodySize() const;
    Location&                        getLocation(const std::string &key);
    std::map<std::string, Location>& getLocations();
    std::map<int, std::string>&      getErrorPages();
    const std::string&               getHost() const;

    // Setters
    void                        setServerName(const std::string &str);
    void                        setHost(const std::string &str); 
    void                        setRoot(const std::string &root);
    void                        setPort(const std::string &port);
    void                        setAutoIndex(const std::string &value);
    void                        setBodySize(const std::string &value);
    void                        setErrorPage(int code, const std::string &path);
    void                        setLocation(const Location& loc);

    // Helper functions
    void                        hasForbiddenSymbols(const std::string &word);
    int                         convertToInt(const std::string &word);
    bool                        hasValidUnit(const std::string &word);
    bool                        isValidDirectory(const std::string& path);
};



#endif
