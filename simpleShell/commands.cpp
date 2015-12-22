// $Id: commands.cpp,v 1.9 2014-03-26 19:55:18-07 - - $

#include <stdexcept>

#include "commands.h"
#include "debug.h"

commands::commands(): map ({
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
   {"rmr"   , fn_rmr   },
}){}

function commands::at (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (function)
   commandmap::const_iterator result = map.find (cmd);
   if (result == map.end()) {
      throw yshell_exn (cmd + ": no such function");
   }
   return result->second;
}


void fn_cat (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size() == 1) {
      throw yshell_exn("Specify file");
      exit_status:: set(1);
      return;
   }
   for(size_t i = 1; i < words.size(); i++) {
      string oriFileName = words.at(i);
      string filename = words.at(i);
      inode* destNode = state.getDesNode(filename);
      if(destNode == NULL) {
         exit_status:: set(1);
         throw yshell_exn
               ( "cat: " + oriFileName + ": No such file or directory");
      }
      directory* destDir = destNode->getNodeDir();
      if(destDir->find(filename) == destDir->end()){
         exit_status:: set(1);
         throw yshell_exn
               ( "cat: " + filename + ": No such file or directory");
      }else{
         inode* temp = (*destDir)[filename];
         if(temp->checknode() == DIR_INODE){
            exit_status:: set(1);
            throw yshell_exn("cat: " + filename + ": Is a directory");
         }
         cout << filename << ":" << endl; 
         wordvec data = temp->readfile();
         state.printData(data);
      }
   }    
}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size() > 2) {
      throw yshell_exn("Specify one directory");
      exit_status:: set(1);
   }
   if(words.size() == 1 || words.at(1) == "/") {
      state.goRoot();
      return;
   }
   string nextDir = words.at(1);
   state.gotoDir(nextDir);
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   wordvec:: const_iterator itor = words.begin();
   wordvec:: const_iterator end = words.end();
   for(; itor != end; ++itor){
      if(itor == words.begin())  continue;
      cout << *itor << " ";
   }
   cout << endl;
}

void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size() != 1) {
      string status = words.at(1);
      int newStatus;
      try {
         newStatus = stoi(status);
      }
      catch(invalid_argument&) {
         throw yshell_exn(status + " isn't a number");
         exit_status:: set(127);
         throw ysh_exit_exn();
      }
      exit_status:: set(newStatus);
   }
   throw ysh_exit_exn();
}

void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size() == 1 || words.at(1) == "/") {
      if(words.size() != 1) { 
         inode* root = state.getroot();
         cout << "/";
         state.printls(root, true);
         return;
      }
      inode* currentDir = state.getcwd();
      cout << "/";
      state.printls(currentDir, true);
      return;
   }
   for(size_t i = 1; i < words.size(); i++){
      string oriFileName = words.at(i);
      string filename = words.at(i);
      inode* destNode = state.getDesNode(filename);
      if(destNode == NULL) {
         exit_status:: set(1);
         throw yshell_exn
               ("ls: " + oriFileName + ": No such file or directory");
      }
      directory* destDir = destNode->getNodeDir();
      if(destDir->find(filename) == destDir->end()){//deal with file too
         exit_status:: set(1);
         throw yshell_exn
               ("ls: " + oriFileName + ": No such file or directory");
      }else{
         inode* temp = (*destDir)[filename];
         if(temp->checknode() == FILE_INODE){
            cout << filename << endl;
            continue;
         }
         if(filename == "." || filename == ".."){
            cout << filename;
            state.printls(temp, false);
         } else {
            cout << "/";
            state.printls(temp, true);
         }
      }
   }
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   string filename = "";
   if(words.size() == 1 || words.at(1) == "/") {
      if(words.size() != 1) {
         filename = words.at(1);
         inode* rootNode = state.getroot();
         state.printlsr(rootNode, filename);
         return;
      }
      inode* curNode = state.getcwd();
      state.printlsr(curNode, "/");
      return;
   }
   for(size_t i = 1; i < words.size(); i++) {
      string oriFileName = words.at(i);
      filename = words.at(i);
      inode* destNode = state.getDesNode(filename);
      if(destNode == NULL) {
         exit_status:: set(1);
         throw yshell_exn
               ("lsr: " + oriFileName + ": No such file or directory");

      } 
      directory* dir = destNode->getNodeDir();
      if(dir->find(filename) == dir->end()){//deal with file
         exit_status:: set(1);
         throw yshell_exn
               ("lsr: " + oriFileName + ": No such file or directory");
      }
      inode* printNode = (*dir)[filename];
      if(printNode->checknode() == FILE_INODE){
            cout << filename << endl;
            continue;
      }
      if(filename == "." || filename == "..") { 
         state.printlsr(printNode, filename);  
      } else {
         state.printlsr(printNode, "/");
      }
   }
}


void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size() == 1) {
      throw yshell_exn ("Specify file");
      exit_status:: set(1);
   }
   string oriFileName = words.at(1);
   string filename = words.at(1);
   inode *destNode = state.getDesNode(filename);
   if(destNode == NULL) {
      exit_status:: set(1);
      throw yshell_exn( "make: " + oriFileName+ ": Is a directory");
   }
   directory* dir = destNode->getNodeDir();
   if(dir->find(filename) != dir->end()){
      inode* temp = (*dir)[filename];
      if(temp->checknode() == DIR_INODE){
         exit_status:: set(1);
         throw yshell_exn( "make: " + oriFileName+ ": Is a directory");
      }
      inode* exists = (*dir)[filename];
      exists->writefile(words);
      return;
   }
   inode *temp = &destNode->mkfile(filename);
   temp->writefile(words);
}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size() == 1) {
      throw yshell_exn("Specify file");
      exit_status:: set(1);
   }
   string oriFileName = words.at(1);
   string filename = words.at(1);
   inode *destNode = state.getDesNode(filename);//deal if already exists
   if(destNode == NULL) {
      throw yshell_exn("mkdir: cannot create directory '"+oriFileName+ 
                       "': No such file or directory");
   }
   directory* dir = destNode->getNodeDir();
   if(dir->find(filename) != dir->end()){
      throw yshell_exn(oriFileName + "already exists");
      exit_status:: set(1);
   }
   inode *temp = &destNode->mkdir(filename);
   (void) temp;
}

void fn_prompt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size() == 1) {
      throw yshell_exn( "Specify prompt" );
      exit_status:: set(1);
   }
   string prompt = "";
   wordvec:: const_iterator itor= words.begin();
   wordvec:: const_iterator end = words.end();
   ++itor;
   for(; itor != end; ++itor){
      prompt = prompt + *itor + " ";
   }
  
   state.setprompt(prompt);
}

void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   string currDir = state.getname();
   cout << "/" << currDir << endl;
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size() == 1) {
      throw yshell_exn("Specify file");
      exit_status:: set(1);
   }
   string oriFileName = words.at(1);
   string filename = words.at(1);
   inode* destNode = state.getDesNode(filename);
   directory *desDir = destNode->getNodeDir();
   if(desDir->find(filename) == desDir->end()){
      exit_status:: set(1);
      throw yshell_exn
            ( "rm: " + oriFileName + ": No such file or directory");
   }
   destNode->remove(filename);
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size() == 1) {
      throw yshell_exn("Specify file");
      exit_status:: set(1);
   }
   string oriFileName = words.at(1);
   string filename = words.at(1);
   inode* destNode = state.getDesNode(filename);
   if(destNode == NULL) {
      exit_status:: set(1);
      throw yshell_exn
            ("rmr: " + oriFileName + ": No such file or directory");
   }
   directory *desDir = destNode->getNodeDir();
   if(desDir->find(filename) == desDir->end()){
      exit_status:: set(1);
      throw yshell_exn
            ("rmr: " + filename + ": No such file or directory");
   } 
   inode* deleteNode = (*desDir)[filename];
   if(deleteNode->checknode() == FILE_INODE) {
      destNode->remove(filename);
      return;
   }
   destNode->rmrRemove(deleteNode);
   directory:: iterator itor = desDir->find(filename);
   desDir->erase(itor);
}

int exit_status_message() {
   int exit_status = exit_status::get();
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}

