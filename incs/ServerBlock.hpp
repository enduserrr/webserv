/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:44:23 by eleppala          #+#    #+#             */
/*   Updated: 2025/02/10 08:56:17 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef SERVERBLOCK_HPP
#define SERVERBLOCK_HPP

#include "ErrorHandler.hpp"
#include <iostream>
#include <vector>
#include "Location.hpp"

class ServerBlock {
private:
    std::string             _serverName;
    std::string             _root;
    std::vector<int>        _ports;
    bool                    _autoIndex;  //1 true - 0 false
    size_t                  _bodySize;
    std::vector<Location>   _locations;

public:
    ServerBlock();
    ~ServerBlock();

    // Getters
    std::string             getServerName() const;
    std::string             getRoot() const;
    std::vector<int>        getPorts() const;
    bool                    getAutoIndex();
    size_t                  getBodySize() const;
    std::vector<Location>&  getLocations();

    // Setters
    void                    setServerName(std::string str);
    void                    setRoot(std::string root);
    void                    setPorts(int port);
    void                    setAutoIndex(bool b);
    void                    setBodySize(size_t size);
    void                    setErrorPages(const std::map<int, std::string> &errorPages);
    void                    setLocation(const Location& loc);

    // void display() const;
};

#endif
