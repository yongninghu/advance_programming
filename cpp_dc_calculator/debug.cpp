// $Id: debug.cpp,v 1.1 2014-04-28 21:35:07-07 - - $
// Partner: Jonathan Lu (jalu@ucsc.edu)
// Partner: Yong Ning Hu (cryhu@ucsc.edu)


#include <cassert>
#include <climits>
#include <iostream>
#include <vector>
using namespace std;

#include "debug.h"
#include "util.h"

vector<bool> debugflags::flags (UCHAR_MAX + 1, false);

void debugflags::setflags (const string& initflags) {
   for (const char flag: initflags) {
      if (flag == '@') flags.assign (flags.size(), true);
                  else flags[flag] = true;
   }
   // Note that DEBUGF can trace setflags.
   if (getflag ('x')) {
      string flag_chars;
      for (size_t index = 0; index < flags.size(); ++index) {
         if (getflag (index)) flag_chars += (char) index;
      }
      DEBUGF ('x', "debugflags::flags = " << flag_chars);
   }
}

//
// getflag -
//    Check to see if a certain flag is on.
//

bool debugflags::getflag (char flag) {
   // WARNING: Don't TRACE this function or the stack will blow up.
   unsigned uflag = (unsigned char) flag;
   assert (uflag < flags.size());
   return flags[uflag];
}

void debugflags::where (char flag, const char* file, int line,
                        const char* func) {
   cout << sys_info::execname() << ": DEBUG(" << flag << ") "
        << file << "[" << line << "] " << func << "()" << endl;
}

