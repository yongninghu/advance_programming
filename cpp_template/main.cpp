// $Id: main.cpp,v 1.4 2014-04-24 18:14:51-07 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

typedef xpair<string,string> str_str_pair;
typedef listmap<string,string> str_str_map;

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            traceflags::setflags (optarg);
            break;
         default:
            complain() << "-" << (char) optopt << ": invalid option"
                       << endl;
            break;
      }
   }
}

string trim (const string &str) {
   size_t first = str.find_first_not_of (" \t");
   if (first == string::npos) return "";
   size_t last = str.find_last_not_of (" \t");
   return str.substr (first, last - first + 1);
}

void insertMap(const string& file,istream& infile,str_str_map& smap) {
   for(int linenr = 1;;++linenr) {
      string line;
      getline(infile, line);
      if(infile.eof()) break;
      line = trim(line);
      cout << file << ": " << linenr << ": " << line << endl;
      if(line.size() == 0 or line[0] == '#') continue;
      size_t pos = line.find_first_of("=");
      if(pos == string::npos) {
         //find the key and printout its value
         str_str_map::iterator itor = smap.find(line);
         if(itor == smap.end()) {
            cout << line << ": key not found" << endl;
            continue;
         }
         cout << itor->first << " = " << itor->second << endl; 
      } else {
         string key = trim(line.substr(0, pos == 0 ? 0 : pos));
         string value = trim(line.substr(pos + 1));
         if(key == "" and value != "") {
            //print key and value
            str_str_map::iterator itor = smap.begin();
            str_str_map::iterator end = smap.end();
            for(; itor != end; ++itor) {
               if(itor->second == value) {
                  cout << itor->first << " = " << itor->second << endl;
               }
            }
            continue;
         }
         if(key != "" and value == "") {
            //erase key and value
            str_str_map::iterator itor = smap.find(key);
            if(itor != smap.end()) itor.erase(); 
            continue;
         }
         if(key == "" and value == "") {
            str_str_map::iterator itor = smap.begin();
            str_str_map::iterator end = smap.end();
            for(; itor != end; ++itor) {
               cout << itor->first << " = " << itor->second << endl;
            }
            continue;
         }
         str_str_pair pair(key, value);
         smap.insert(pair);
         cout << key << " = " << value << endl;
      }
   }
}

int main (int argc, char** argv) {
   sys_info::set_execname (argv[0]);
   scan_options (argc, argv);

   str_str_map smap;

   if(optind == argc) {
      insertMap("-", cin, smap);
   } else {
      for(int i = 1; i < argc; ++i) {
         string filename = argv[i];
         if(filename == "-") {
            insertMap("-", cin, smap);
            continue;
         }
         ifstream file(filename);
         if(file.fail()) {
            syscall_error(filename);
            continue;
         }
         insertMap(filename, file, smap);
         file.close();
      }
   }
  
   sys_info::clear();
   return sys_info::get_exit_status();
}

