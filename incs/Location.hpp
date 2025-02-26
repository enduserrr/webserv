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
#include <map>

class Location {
private:
    std::string                 _path;
    std::vector<std::string>    _allowedMethods;
    bool                        _autoIndex;
    std::string                 _root;
    std::map<int, std::string>  _errorPages;
    std::pair<int, std::string> _redirect; 

public:
    Location();
    ~Location();

    //getters
    std::string                 getPath() const;
    std::string&                getRoot();
    bool&                       getAutoIndex(); 
    std::vector<std::string>    getAllowedMethods() const;
    std::map<int, std::string>& getErrorPages();
    std::pair<int, std::string> getRedirect() const ;

    //setters
    void                        setPath(const std::string& path);
    void                        setRoot(std::string root);
    void                        setAutoIndex(bool b);
    void                        addAllowedMethod(const std::string& method);
    void                        setErrorPage(int &code, std::string& path);
    void                        setRedirect(int code, const std::string& url); 

};

#endif
