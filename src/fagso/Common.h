/* This file is part of FAGSO, a program to find network partitions
*
*  Copyright (C) 2014-2015 Carlo Nicolini <carlo.nicolini@iit.it>
*
*  Surprise++ is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License as published by the Free Software Foundation; either
*  version 3 of the License, or (at your option) any later version.
*
*  Alternatively, you can redistribute it and/or
*  modify it under the terms of the GNU General Public License as
*  published by the Free Software Foundation; either version 2 of
*  the License, or (at your option) any later version.
*
*  Surprise++ is distributed in the hope that it will be useful, but WITHOUT ANY
*  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
*  FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public
*  License and a copy of the GNU General Public License along with
*  Surprise++. If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef _COMMON_H
#define	_COMMON_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <string>

using namespace std;
/**
*  \defgroup Common The Common group which encapsulates some common usage functions
*  \brief Common groups all the functions and methods which are of very common use, they are at most static methods.
*  \class Common
* \ingroup Common
* \brief Common is a set of functions and method for various usage
**/
namespace Common
{
void skipSpaces(stringstream &str);
void readChar(stringstream &str, int c);
void skipSpacesReadChar(stringstream &str, int c);
int nextChar(stringstream &str);
void tokenizeString(const string& str, vector<string>& tokens, const string& delimiters);
string nextToken(stringstream &str, const char *delimiters);
string trim(string &s);
int equalCaseInsensitive(string s1, string s2);
bool  isnumeric(const char *p);

// Old versions of IO functions
int iReadChar(stringstream &str, int c);
int iSkipSpacesReadChar(stringstream &str, int c);
int iNextChar(stringstream &str);
};

template <class T> string stringify(const T &t)
{
    std::ostringstream o;
    o<< t;
    return o.str();
}

template< class T>
bool str2num( const std::string& s, T &val )
{
    std::istringstream i(s);
    if (!(i >> val))
        return false;
    return true;
}

#endif	/* _COMMON_H */

