/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eleppala <eleppala@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:43:41 by eleppala          #+#    #+#             */
/*   Updated: 2025/01/28 18:43:43 by eleppala         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

// Constructor
Location::Location(){}

// Destructor
Location::~Location() {}

// // Copy Constructor
// Location::Location(const Location &other) {
//     *this = other;
// }

// // Copy Assignment Operator
// Location& Location::operator=(const Location &other) {
//     if (this != &other) {
//         this->value = other.value;
//     }
//     return *this;
// }

// // Getters and Setters
// int Location::getValue() const {
//     return value;
// }

bool Location::setPath(const std::string& path) {
    // std::cout << "setpath: "<< path << std::endl; 
    _path = path;
    return true;
}

bool Location::addAllowedMethod(const std::string& method){
    if (method != "GET" && method != "POST" && method != "DELETE")
        return false;  
    _allowedMethods.push_back(method);
    return true; 
}

std::string Location::getPath() const {
    // std::cout << "getpath: "<< _path << std::endl; 
    return _path;
}

std::vector<std::string> Location::getAllowedMethods() const {
    return _allowedMethods;
}




// // Member function
// void Location::display() const {
//     std::cout << "Value: " << value << std::endl;
// }