// $Id: cix.cpp,v 1.9 2019-04-05 15:04:28-07 - - $
// Jeffrey Yao jeyao
// Herman Wu hwwu

#include <iostream>
#include <memory>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"
#include "logstream.h"
#include "sockets.h"

logstream outlog (cout);
struct cix_exit: public exception {};

unordered_map<string,cix_command> command_map {
   {"exit", cix_command::EXIT},
   {"help", cix_command::HELP},
   {"ls"  , cix_command::LS  },
   {"get"  , cix_command::GET  },
   {"put"  , cix_command::PUT  },
   {"rm"  , cix_command::RM  },
};

static const char help[] = R"||(
exit         - Exit the program.  Equivalent to EOF.
get filename - Copy remote file to local host.
help         - Print help summary.
ls           - List names of files on remote server.
put filename - Copy local file to remote host.
rm filename  - Remove file from remote server.
)||";

void cix_help() {
   cout << help;
}

void cix_ls (client_socket& server) {
   cix_header header;
   header.command = cix_command::LS;
   outlog << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   outlog << "received header " << header << endl;
   if (header.command != cix_command::LSOUT) {
      outlog << "sent LS, server did not return LSOUT" << endl;
      outlog << "server returned " << header << endl;
   }
   else {
      auto buffer = make_unique<char[]> (header.nbytes + 1);
      recv_packet (server, buffer.get(), header.nbytes);
      outlog << "received " << header.nbytes << " bytes" << endl;
      buffer[header.nbytes] = '\0';
      cout << buffer.get();
   }
}

void cix_get (client_socket& server, string& name) {
   cix_header header;
   header.command = cix_command::GET;
   strncpy(header.filename,name.c_str(),FILENAME_SIZE);

   outlog << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   outlog << "received header " << header << endl;

   if (header.command == cix_command::NAK){
      cerr << "server returned " << header << endl;
      cerr << "did not get" << endl;
   }else if (header.command == cix_command::FILEOUT) {
      auto buffer = new char[header.nbytes + 1];
      buffer[header.nbytes] = '\0';
      recv_packet (server, buffer, header.nbytes);
      outlog << "received " << header.nbytes << " bytes" << endl;

      ofstream recp(name, ofstream::binary);
      recp.write(buffer, header.nbytes);
      recp.close();

      outlog << "successfully received " << header << endl;
      outlog << "received " << header.nbytes << " bytes" << endl;
   }
   else {
      outlog << "sent GET, server did not return FILEOUT" << endl;
      outlog << "server returned " << header << endl;
   }

}

void cix_put (client_socket& server, string name) {
   cix_header header;
   header.command = cix_command::PUT;
   strncpy(header.filename,name.c_str(),FILENAME_SIZE);

   ifstream ifile(name, ifstream::binary);
   if(ifile.is_open()){
      ifile.seekg (0, ifile.end);
      int length = ifile.tellg();
      ifile.seekg (0, ifile.beg);

    // allocate memory:
      char * buffer = new char [length];

    // read data as a block:
      ifile.read (buffer,length);

      ifile.close();
      header.nbytes = length;
      outlog << "sending header " << header << endl;
      send_packet (server, &header, sizeof header);
      send_packet (server, buffer, length);
      recv_packet (server, &header, sizeof header);
      outlog << "received header " << header << endl;
   }else{
      outlog << header.filename << " doesn't exist" << endl;
      return;
   }

   if (header.command == cix_command::ACK) {
      outlog << "successfully sent " << header << endl;
      outlog << "sent " << header.nbytes << " bytes" << endl;
   }else if(header.command == cix_command::NAK){
      cerr << "server returned " << header << endl;
      cerr << "did not put" << endl;
   }
   else {
      outlog << "sent PUT, server did not return ACK or NAK" << endl;
      outlog << "server returned " << header << endl;
   }
}

void cix_rm (client_socket& server, string& name) {
   cix_header header;
   header.command = cix_command::RM;

   strncpy(header.filename,name.c_str(),FILENAME_SIZE);
   header.nbytes = 0;

   outlog << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   outlog << "received header " << header << endl;

   if (header.command == cix_command::ACK) {
      outlog << "server returned " << header << endl;
      outlog << "succssfully deleted " << header << endl;
   }
   else if(header.command == cix_command::NAK){
      cerr << "server returned " << header << endl;
      cerr << "did not delete " << header << endl;
   }
   else {
      outlog << "sent RM, server did not return ACK or NAK" << endl;
      outlog << "server returned " << header << endl;
   }
}

void usage() {
   cerr << "Usage: " << outlog.execname() << " [host] [port]" << endl;
   throw cix_exit();
}

int main (int argc, char** argv) {
   outlog.execname (basename (argv[0]));
   outlog << "starting" << endl;
   vector<string> args (&argv[1], &argv[argc]);
   if (args.size() > 2) usage();
   string host = get_cix_server_host (args, 0);
   in_port_t port = get_cix_server_port (args, 1);
   outlog << to_string (hostinfo()) << endl;
   try {
      outlog << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      outlog << "connected to " << to_string (server) << endl;
      for (;;) {
         string line;
         string file;
         string command;

         getline (cin, line);
         if (cin.eof()) throw cix_exit();
         
         outlog << "command " << line << endl;
         vector<string> cline = split(line, " ");
         command = cline.at(0);
         if(cline.size() > 1){
            file = cline.at(1);
            outlog << "filename: " << file << endl;
         }

         const auto& itor = command_map.find (command);
         cix_command cmd = itor == command_map.end()
                         ? cix_command::ERROR : itor->second;
         switch (cmd) {
            case cix_command::EXIT:
               throw cix_exit();
               break;
            case cix_command::HELP:
               cix_help();
               break;
            case cix_command::LS:
               cix_ls (server);
               break;
            case cix_command::GET:
               cix_get(server, file);
               break;
            case cix_command::PUT:
               cix_put(server, file);
               break;
            case cix_command::RM:
               cix_rm(server, file);
               break;
            default:
               outlog << line << ": invalid command" << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      outlog << error.what() << endl;
   }catch (cix_exit& error) {
      outlog << "caught cix_exit" << endl;
   }
   outlog << "finishing" << endl;
   return 0;
}

