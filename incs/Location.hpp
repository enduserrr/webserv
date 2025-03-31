/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:44:35 by eleppala          #+#    #+#             */
/*   Updated: 2025/03/16 13:01:27 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "Libs.hpp"


class Location {
    private:
        std::string                 _path;
        std::string                 _index;
        std::string                 _root;
        std::string                 _uploadStore;
        bool                        _autoIndex;
        std::vector<std::string>    _allowedMethods;
        std::map<int, std::string>  _errorPages;
        std::pair<int, std::string> _redirect;

    public:
        Location();
        ~Location();

        //getters
        std::string                 getPath() const;
        std::string&                getRoot();
        bool&                       getAutoIndex();
        const std::string&          getIndex() const;
        const std::string&          getUploadStore() const;
        std::vector<std::string>    getAllowedMethods() const;
        std::map<int, std::string>& getErrorPages();
        std::pair<int, std::string> getRedirect() const ;

        //setters
        void                        setPath(const std::string& path);
        void                        setRoot(const std::string &root);
        void                        setIndex(const std::string &name);
        void                        setUploadStore(const std::string &path);
        void                        setAutoIndex(const std::string &value);
        void                        addAllowedMethod(const std::string &method);
        void                        setErrorPage(int code, const std::string &path);
        void                        setRedirect(int code, const std::string& url);

};

#endif
