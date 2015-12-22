// $Id: inode.cpp,v 1.3 2014-03-26 18:39:40-07 - - $

#include <cassert>
#include <iostream>

using namespace std;

#include "debug.h"
#include "inode.h"

int inode::next_inode_nr {1};

inode::inode(inode_t init_type):
   inode_nr (next_inode_nr++), type (init_type)
{
   switch (type) {
      case DIR_INODE:
           contents.dirents = new directory();
           break;
      case FILE_INODE:
           contents.data = new wordvec();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

//
// copy ctor -
//    Make a copy of a given inode.  This should not be used in
//    your program if you can avoid it, since it is expensive.
//    Here, we can leverage operator=.
//
inode::inode (const inode& that) {
   *this = that;
}

//
// operator= -
//    Assignment operator.  Copy an inode.  Make a copy of a
//    given inode.  This should not be used in your program if
//    you can avoid it, since it is expensive.
//
inode& inode::operator= (const inode& that) {
   if (this != &that) {
      inode_nr = that.inode_nr;
      type = that.type;
      contents = that.contents;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
   return *this;
}


int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

int inode::size() const {
   int size {0};
   int count = 0;
   DEBUGF ('i', "size = " << size);
   string res = "";
   switch (this->type) {
      case DIR_INODE:
           size = this->contents.dirents->size();
           break;
      case FILE_INODE:
           wordvec:: iterator itor = this->contents.data->begin();
           wordvec:: iterator end = this->contents.data->end();
           for(; itor != end; ++itor){
              count++;
              if(count <= 2) continue;
              res = res + *itor;
           }
           size = res.length() + count - 3;
           if(size < 0) return 0;
           break;
   } 
   return size;
}

const wordvec& inode::readfile() const {
   DEBUGF ('i', *contents.data);
   assert (type == FILE_INODE);
   return *contents.data;
}

int inode:: checknode() {
   return this->type;
}

directory* inode:: getNodeDir(){
   return this->contents.dirents;
}

void inode::writefile (const wordvec& words) {
   DEBUGF ('i', words);
   assert (type == FILE_INODE);
   wordvec *newWords = new wordvec(words);
   this->contents.data = newWords;
}

void inode::remove (const string& filename) {
   DEBUGF ('i', filename);
   assert (type == DIR_INODE);
   directory* it = this->contents.dirents;
   inode* fileNode = (*it)[filename];
   if(fileNode->type == DIR_INODE) {
      if(fileNode->size() > 2) {
         exit_status:: set(1);
         throw yshell_exn ("rm: failed to remove '" + filename 
                          + "': Directory not empty");
         return;
      }
   }
   directory:: iterator itor;
   itor = it->find(filename);
   it->erase(itor);
}

void inode:: rmrRemove (inode* delNode) {
   directory *curDir = delNode->contents.dirents;
   directory:: iterator itr = curDir->begin();
   while(itr != curDir->end()) {
      if(itr->second->type == FILE_INODE) {
         directory:: iterator toErase = itr;
         ++itr;
         curDir->erase(toErase);
      }else {
         if(itr->first == "." || itr->first == "..") {
            ++itr;
            continue;
         }
         delNode->rmrRemove(itr->second);
         directory:: iterator itor = itr;
         curDir->erase(itor);
         ++itr;
      }
   }
}

inode& inode::mkdir (const string& dirname) {
   inode *newDir = new inode(DIR_INODE);
   newDir->nodeName = dirname;
   (*newDir->contents.dirents)["."] = newDir;
   (*newDir->contents.dirents)[".."] = this;
   (*this->contents.dirents)[dirname] = newDir;
   return *newDir;
}

inode& inode::mkfile (const string& filename) {
   inode *newFile = new inode(FILE_INODE);
   newFile->nodeName = filename;
   (*this->contents.dirents)[filename] = newFile;
   return *newFile;
}

inode_state::inode_state() {
   DEBUGF ('i', "root = " << (void*) root << ", cwd = " << (void*) cwd
          << ", prompt = " << prompt);
   root = new inode(DIR_INODE);
   root->nodeName = "";
   string rootName = ".";
   string rootParent = "..";
   (*root->contents.dirents)[rootName] = root;
   (*root->contents.dirents)[rootParent] = root;
   cwd = root;
}

inode* inode_state:: getcwd() {
   return cwd;
}

inode* inode_state:: getroot() {
   return root;
}

void inode_state:: printData(const wordvec& words) {
   int count = 0;
   wordvec:: const_iterator itor = words.begin();
   wordvec:: const_iterator end = words.end();
   for(; itor != end; ++itor){
      count++;
      if(count <= 2) continue;
      cout << *itor << " ";
   }
   cout << endl;
}

directory* inode_state:: curDir() {
   return cwd->contents.dirents;
}

void inode_state:: printls(inode *currentDir, bool printName) {
   cout << (printName==true ? currentDir->nodeName:"") << ":" << endl;
   directory *curDir = currentDir->contents.dirents;
   for(auto it = curDir->cbegin(); it != curDir->cend(); ++it) {
      cout.width(6); cout << right << it->second->inode_nr;
      cout.width(8); cout << right << 
      it->second->size();
      cout << "  " << it->first;
      if(it->second->type == DIR_INODE && 
         it->first != "." && it->first !="..") cout << "/";
      cout << endl;
   }
}

void inode_state:: printlsr(inode* currentDir, string prevDir) {
   cout << prevDir << 
        (prevDir == "." || prevDir == ".." ? "":currentDir->nodeName)
        << ":" << endl;
   prevDir = prevDir + currentDir->nodeName;
   directory *curDir = currentDir->contents.dirents;
   for(auto it = curDir->cbegin(); it != curDir->cend(); ++it) {
      cout.width(6); cout << right << it->second->inode_nr;
      cout.width(8); cout << right <<
      it->second->size();
      cout << "  " << it->first;
      if(it->second->type == DIR_INODE &&
         it->first != "." && it->first !="..") {
         cout << "/";
      }
      cout << endl;
   }
   for(auto it = curDir->cbegin(); it != curDir->cend(); ++it) {
      if(it->second->type == DIR_INODE &&
         it->first != "." && it->first !="..") {
         this->printlsr(it->second, prevDir+(prevDir == "/" ? "":"/"));
      }
   }
}

void inode_state:: setprompt(const string& prompt){
   this->prompt.erase();
   this->prompt.append(prompt);
}

string& inode_state:: getprompt() {
   return this->prompt;
}

string& inode_state:: getname() {
   return this->cwd->nodeName;
}

void inode_state:: goRoot() {
   cwd = root;
}

void inode_state:: gotoDir(const string& nextDir) {
   //directory *currDir = cwd->contents.dirents;
   string filename = nextDir;
   inode* destNode = this->getDesNode(filename);
   if(destNode == NULL) throw yshell_exn("cd: " + nextDir + 
                                 ": Not a directory");
   directory* destDir = destNode->contents.dirents;
   if(destDir->find(filename) == destDir->end()){
      exit_status:: set(1);
      throw yshell_exn( "cd: " + nextDir +
                       ": No such file or directory");
   } else {
      inode* temp = (*destDir)[filename];
      if(temp->type == DIR_INODE) {
         cwd = temp;
      } else {
         exit_status:: set(1);
         throw yshell_exn( "cd: " + nextDir + ": Not a directory");
      }
   }
}

inode* inode_state:: getDesNode(string& path) {
   wordvec words = split(path, "/");
   inode* curNode = cwd;
   for(size_t i = 0; i < words.size() - 1; i++) {
      string filename = words.at(i);
      directory* curDir = curNode->contents.dirents;
      if(curDir->find(filename) == curDir->end()){
         exit_status:: set(1);
         return NULL;
      }
      curNode = (*curDir)[filename];
      if(curNode->type == FILE_INODE) return NULL;
   }
   path = words.at(words.size()-1);
   return curNode;
}

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

