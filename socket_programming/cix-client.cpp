// $Id: cix-client.cpp,v 1.4 2014-05-30 12:47:58-07 - - $

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "cix_protocol.h"
#include "logstream.h"
#include "signal_action.h"
#include "sockets.h"
#include "util.h"

logstream log (cout);

unordered_map<string,cix_command> command_map {
   {"exit", CIX_EXIT},
   {"help", CIX_HELP},
   {"ls"  , CIX_LS  },
   {"get" , CIX_GET },
   {"put" , CIX_PUT },
   {"rm"  , CIX_RM  },
};

void cix_help() {
   static vector<string> help = {
      "exit         - Exit the program.  Equivalent to EOF.",
      "get filename - Copy remote file to local host.",
      "help         - Print help summary.",
      "ls           - List names of files on remote server.",
      "put filename - Copy local file to remote host.",
      "rm filename  - Remove file from remote server.",
   };
   for (const auto& line: help) cout << line << endl;
}

void cix_ls (client_socket& server) {
   cix_header header;
   header.cix_command = CIX_LS;
   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;
   if (header.cix_command != CIX_LSOUT) {
      log << "sent CIX_LS, server did not return CIX_LSOUT" << endl;
      log << "server returned " << header << endl;
   }else {
      char buffer[header.cix_nbytes + 1];
      recv_packet (server, buffer, header.cix_nbytes);
      log << "received " << header.cix_nbytes << " bytes" << endl;
      buffer[header.cix_nbytes] = '\0';
      cout << buffer;
   }
}

// assumes the filename is valid (length <= 58, no "/")
void cix_get (client_socket& server, string filename) {
   // prepare the header
   cix_header header;
   header.cix_command = CIX_GET;
   strncpy(header.cix_filename, filename.c_str(),
      sizeof(header.cix_filename));
   header.cix_filename[filename.size()] = '\0';

   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;

   // process the response
   if(header.cix_command != CIX_FILE) {
      log << "get failed for '" << filename << "': " <<
         strerror(header.cix_nbytes) << endl;
   } else {
      char buffer[header.cix_nbytes + 1];
      recv_packet (server, buffer, header.cix_nbytes);
      log << "received " << header.cix_nbytes << " bytes" << endl;
      buffer[header.cix_nbytes] = '\0';
      
      ofstream outfile (header.cix_filename, ofstream::binary);
      if(outfile.fail()) {
         log << header.cix_filename << ": " << strerror(errno) << endl;
         return;
      }
      outfile.write(buffer, header.cix_nbytes);
      outfile.close();
   }
}

// assumes valid filename
void cix_put (client_socket& server, string filename) {
   // prepare the file data
   ifstream infile (filename, ifstream::binary);
   if(infile.fail()) {
      log << filename << ": opening the file failed" << endl;
      return;
   }
   infile.seekg(0, infile.end);
   int length = infile.tellg();
   infile.seekg(0, infile.beg);
   
   // prepare the header
   cix_header header;
   header.cix_command = CIX_PUT;
   strncpy(header.cix_filename, filename.c_str(),
      sizeof(header.cix_filename));
   header.cix_filename[filename.size()] = '\0';
   header.cix_nbytes = length;

   // read in file data
   char buffer[length];
   infile.read(buffer, length);
   infile.close();

   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   send_packet (server, buffer, length);
   log << "sent " << length << " bytes" << endl;

   recv_packet (server, &header, sizeof header);
   
   // process the response
   if(header.cix_command != CIX_ACK)
      log << "cannot put '" << filename << "': " <<
         strerror(header.cix_nbytes) << endl;
   else
      log << "put '" << filename << "'" << endl;
}

void cix_rm (client_socket& server, string filename) {
   // prepare the header
   cix_header header;
   header.cix_command = CIX_RM;
   strncpy(header.cix_filename, filename.c_str(),
      sizeof(header.cix_filename));
   header.cix_filename[filename.size()] = '\0';

   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   
   // process the response
   if(header.cix_command != CIX_ACK)
      log << "cannot remove '" << filename << "': " <<
         strerror(header.cix_nbytes) << endl;
   else
      log << "removed '" << filename << "'" << endl;
}


void usage() {
   cerr << "Usage: " << log.execname() << " [host] [port]" << endl;
   throw cix_exit();
}

void signal_handler (int signal) {
   log << "signal_handler: caught " << strsignal (signal) << endl;
   switch (signal) {
      case SIGINT: case SIGTERM: throw cix_exit();
      default: break;
   }
}

int main (int argc, char** argv) {
   log.execname (basename (argv[0]));
   log << "starting" << endl;
   vector<string> args (&argv[1], &argv[argc]);
   signal_action (SIGINT, signal_handler);
   signal_action (SIGTERM, signal_handler);
   if (args.size() > 2) usage();
   
   string host = "localhost";
   in_port_t port = 65535;

   
   if(args.size() == 1 &&
   args[0].find_first_not_of("0123456789") == string::npos) {
      port = get_cix_server_port(args, 0);
   } else {
      host = get_cix_server_host (args, 0);
      port = get_cix_server_port (args, 1);
   }

   log << to_string (hostinfo()) << endl;
   try {
      log << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      log << "connected to " << to_string (server) << endl;
      for (;;) {
         string line;
         getline (cin, line);
         if (cin.eof()) throw cix_exit();

         // split the line into commands and arguments
         trim_spaces(line);
         list<string> words = split(line, " ");

         // get the command
         string cmdkey = words.front();
         trim_spaces(cmdkey);

         log << "command " << cmdkey << endl;
         const auto& itor = command_map.find (cmdkey);
         cix_command cmd = itor == command_map.end()
                         ? CIX_ERROR : itor->second;
         switch (cmd) {
            case CIX_EXIT:
               throw cix_exit();
               break;
            case CIX_HELP:
               cix_help();
               break;
            case CIX_LS:
               cix_ls (server);
               break;
            case CIX_GET:
               // need a filename for get command
               if(words.size() == 2) {
                  string filename = words.back();
                  if(filename_valid(filename))
                     cix_get(server, filename);
               } else {
                  log << line << ": provide filename" << endl;
               }
               break;
            case CIX_PUT:
               if(words.size() == 2) {
                  string filename = words.back();
                  if(filename_valid(filename))
                     cix_put(server, filename);
               } else {
                  log << line << ": provide filename" << endl;
               }
               break;
            case CIX_RM:
               if(words.size() == 2) {
                  string filename = words.back(); 
                  if(filename_valid(filename))
                     cix_rm(server, filename);
               } else {
                  log << line << ": provide filename" << endl;
               }
               break;
            default:
               log << line << ": invalid command" << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      log << error.what() << endl;
   }catch (cix_exit& error) {
      log << "caught cix_exit" << endl;
   }
   log << "finishing" << endl;
   return 0;
}

