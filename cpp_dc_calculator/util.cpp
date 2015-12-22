// $Id: util.cpp,v 1.1 2014-04-28 21:35:08-07 - - $
// Partner: Jonathan Lu (jalu@ucsc.edu)
// Partner: Yong Ning Hu (cryhu@ucsc.edu)

#include <cstdlib>
#include <sstream>
using namespace std;

#include "util.h"

ydc_exn::ydc_exn (const string& what): runtime_error (what) {
}

const string octal (long decimal) {
   ostringstream ostring;
   ostring.setf (ios::oct);
   ostring << decimal;
   return ostring.str();
}

string sys_info::execname_; // Must be initialized from main().
int sys_info::status_ = EXIT_SUCCESS;

void sys_info::execname (const string& argv0) {
   execname_ = argv0;
   cout << boolalpha;
   cerr << boolalpha;
   DEBUGF ('Y', "execname = " << execname_);
}

ostream& complain() {
   sys_info::status (EXIT_FAILURE);
   cerr << sys_info::execname() << ": ";
   return cerr;
}

