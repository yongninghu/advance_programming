// $Id: util.h,v 1.3 2014-04-24 18:14:51-07 - - $

//
// util -
//    A utility class to provide various services not conveniently
//    associated with other modules.
//

#ifndef __UTIL_H__
#define __UTIL_H__

#include <iostream>
#include <list>
#include <string>

using namespace std;
//
// filename_Valid
//    Determines whether a filename argument is valid.
//    Filenames for this program must be 58 characters max
//    and must not contain any '/' characters.
//    Does not check for null terminating, it is added automatically.

bool filename_valid (const string &line);

//
// split -
//    Split a string into a list<string>..  Any sequence
//    of chars in the delimiter string is used as a separator.  To
//    Split a pathname, use "/".  To split a shell command, use " ".
//

list<string> split (const string& line, const string& delimiter);

//
// trim_spaces -
//    Trim leading and trailing spaces in a given line.
//
void trim_spaces(string& line);

#endif

