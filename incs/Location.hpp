/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:44:35 by eleppala          #+#    #+#             */
/*   Updated: 2025/02/06 08:59:44 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <vector>

class Location {
private:
    std::string                 _path;
    std::vector<std::string>    _allowedMethods;
    // bool                        _dirListing; // gpt oli sita mielta etta 
    bool                        _autoIndex;     // virallinen termi ois autoindex 
    std::string                 _root;

public:
    Location();
    ~Location();

    //getters
    std::string                 getPath() const;
    std::string                 getRoot() const;
    bool                        getAutoIndex(); 
    std::vector<std::string>    getAllowedMethods() const;

    //setters
    void                        setPath(const std::string& path);
    void                        setRoot(std::string root);
    void                        setAutoIndex(bool b);
    bool                        addAllowedMethod(const std::string& method);

};

#endif
