/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eleppala <eleppala@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:44:23 by eleppala          #+#    #+#             */
/*   Updated: 2025/01/28 18:44:25 by eleppala         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERBLOCK_HPP
#define SERVERBLOCK_HPP

#include "ErrorHandler.hpp"
#include <iostream>
#include <vector>
#include "Location.hpp"

#define DEFAULT_BODY_SIZE 1048576  //1MB

class ServerBlock {
private:
    std::string                 _serverName;
    std::string                 _root; 
    std::vector<int>            _ports;
    bool                        _autoIndex;  //1 true - 0 false
    size_t                      _bodySize;
    std::vector<Location>       _locations;
    std::map<int, std::string>  _errorPages; //key value 404 - 404.html --> _errorPages[404] = 404.html
    

public:
    ServerBlock();
    ~ServerBlock();

    // Getters
    std::string                 getServerName() const;
    std::string&                getRoot();
    std::vector<int>            getPorts();
    bool&                       getAutoIndex();
    size_t                      getBodySize() const;
    std::vector<Location>&      getLocations();
    std::map<int, std::string>& getErrorPages();

    // Setters
    void                        setServerName(std::string str);
    void                        setRoot(std::string root);
    void                        setPorts(int port);
    void                        setAutoIndex(bool b);
    void                        setBodySize(int size, char unit);
    void                        setErrorPage(int &code, std::string &path);
    void                        setLocation(const Location& loc);

    // void display() const;
};

#endif
