/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:44:23 by eleppala          #+#    #+#             */
/*   Updated: 2025/03/31 10:43:13 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef SERVERBLOCK_HPP
#define SERVERBLOCK_HPP

#include "Logger.hpp"
#include <iostream>
#include <vector>
#include "Location.hpp"

#define DEFAULT_BODY_SIZE 1048576  //1MB

class ServerBlock {
private:
    std::string                 _serverName;
    std::string                 _root;
    std::string                 _index;
    bool                        _autoIndex;  //1 true - 0 false
    size_t                      _bodySize;
    std::vector<int>            _ports;
    std::map<std::string, Location> _locations;
    std::map<int, std::string>  _errorPages; //key value 404 - 404.html --> _errorPages[404] = 404.html


public:
    ServerBlock();
    ~ServerBlock();

    // Getters
    std::string                 getServerName() const;
    std::string&                getRoot();
    std::vector<int>            getPorts();
    const std::string&          getIndex() const;
    bool&                       getAutoIndex(const std::string &key);
    size_t                      getBodySize() const;
    Location&                   getLocation(const std::string &key);
    std::map<int, std::string>& getErrorPages();

    // Setters
    void                        setServerName(const std::string &str);
    void                        setRoot(const std::string &root);
    void                        setPort(const std::string &port);
    void                        setAutoIndex(const std::string &value);
    void                        setIndex(const std::string &name);
    void                        setBodySize(const std::string &value);
    void                        setErrorPage(int code, const std::string &path);
    void                        setLocation(const Location& loc);

    // void display() const;
};

void                            hasForbiddenSymbols(const std::string &word);
int                             convertToInt(const std::string &word);
bool                            hasValidUnit(const std::string &word);

#endif
