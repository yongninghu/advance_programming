// $Id: util.cpp,v 1.4 2014-04-24 18:14:51-07 - - $

#include <cstdlib>
#include <cstring>
#include <string>
using namespace std;

#include "util.h"

bool filename_valid (const string& name) {
   return name.size() <= 58 && name.find_first_of("/") == string::npos;
}

void trim_spaces (string& line) {
   if(line.size() == 0) {
      line = "";
      return;
   }

   // trailing and leading spaces
   size_t startpos = line.find_first_not_of(" \t");
   size_t endpos = line.find_last_not_of(" \t");

   // check for empty (i.e all spaces)
   if(string::npos == startpos || string::npos == endpos) line = "";
   else line = line.substr(startpos, endpos - startpos + 1);
}


list<string> split (const string& line, const string& delimiters) {
   list<string> words;
   size_t end = 0;
   // Loop over the string, splitting out words, and for each word
   // thus found, append it to the output list<string>.
   for (;;) {
      size_t start = line.find_first_not_of (delimiters, end);
      if (start == string::npos) break;
      end = line.find_first_of (delimiters, start);
      words.push_back (line.substr (start, end - start));
   }
   return words;
}
