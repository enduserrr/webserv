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
    std::string     _serverName;
    std::string     _port;         // str now, but maybe int?
    std::string     _bodySize;

    // std::vector<Location>    _Locations;

public:
    ServerBlock();
    ~ServerBlock();

    // Getters and Setters
    void        setServerName(std::string str);
    void        setPort(std::string str);
    void        setBodySize(std::string str);
    void        setErrorPages(const std::map<int, std::string> &errorPages);

    std::string getServerName() const;
    std::string getPort() const;
    std::string getBodySize() const;
    // void display() const;
};

#endif
