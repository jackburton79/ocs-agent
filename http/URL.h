/*
 * URL.h
 *
 *  Created on: 12/07/2017
 *  Copyright 2017 Stefano Ceccherini (stefano.ceccherini@gmail.com)
 *  The API is inspired from Haiku's BURL class
 */

#ifndef URL_H
#define URL_H

#include <string>

class URL {
public:
    URL(const char* url);
    void SetTo(const char* url);
    
    std::string URLString() const;
    std::string Protocol() const;
    std::string Host() const;
    int Port() const;
    std::string Path() const;

private:
    void _DecodeURLString(const char* urlString);
    
    std::string fURLString;
    std::string fProtocol;
    std::string fHost;
    int fPort;
    std::string fPath;
};

#endif // URL_H
