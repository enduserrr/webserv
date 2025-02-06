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

class ServerBlock {
private:
    std::string             _serverName;
    std::vector<int>        _ports; 
    size_t                  _bodySize;
    std::vector<Location>   _locations;

public:
    ServerBlock();
    ~ServerBlock();

    // Getters and Setters
    void                    setServerName(std::string str);
    void                    setPorts(int port);
    void                    setBodySize(size_t size);
    void                    setErrorPages(const std::map<int, std::string> &errorPages);
    void                    setLocation(const Location& loc);

    std::string             getServerName() const;
    std::vector<int>        getPorts();
    size_t                  getBodySize() const;
    std::vector<Location>&  getLocations();

    // void display() const;
};

#endif
