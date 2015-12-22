// $Id: listmap.tcc,v 1.1 2014-04-24 18:02:55-07 - - $

#include "listmap.h"
#include "trace.h"

template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::node::node (const value_type& pair):
            pair(pair), prev(NULL), next(NULL) {
}

template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::listmap (): head(NULL), tail (NULL) {
}

template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::~listmap () {
   TRACE ('l', (void*) this);
   listmap::iterator itor = this->begin();
   listmap::iterator end = this->end();
   while(itor != end) {
      listmap::iterator tmp = itor;
      ++itor;
      tmp.erase();
   }
}

template <typename Key, typename Value, class Less>
void listmap<Key,Value,Less>::insert (const xpair<Key,Value>& pair) {
   TRACE ('l', &pair << "->" << pair);
   node* newNode = new node(pair);
   newNode->prev = NULL; 
   newNode->next = NULL;
   if(head == NULL) {
      head = tail = newNode;
      return;
   }
   node* currNode = head;
   while(currNode != NULL) {
      if(less(newNode->pair.first, currNode->pair.first)){
         if(currNode == head) {
            head = newNode;
            head->next = currNode;
            currNode->prev = newNode;
            return;
         } else {
            currNode->prev->next = newNode;
            newNode->prev = currNode->prev;
            newNode->next = currNode;
            currNode->prev = newNode;
            return;
         }
      } else if(less(currNode->pair.first, newNode->pair.first)){
         if(currNode == tail) {
            tail = newNode;
            newNode->prev = currNode;
            currNode->next = newNode;
            return;
         } else {
            currNode = currNode->next; 
         }  
      }
      if(not less(newNode->pair.first, currNode->pair.first) and
         not less(currNode->pair.first, newNode->pair.first)) {
         currNode->pair = newNode->pair;
         delete newNode;
         return;
      }
   }
}

template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::find (const key_type& that) {
   TRACE ('l', that);
   node* currNode = head;
   while(currNode != NULL) {
      if(not less(currNode->pair.first, that) &&
         not less(that, currNode->pair.first)) {
         return iterator(this, currNode);
      }
      currNode = currNode->next;
   }
   return iterator(this, NULL);
}

template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::begin () {
   return iterator (this, head);
}

template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::end () {
   return iterator (this, NULL);
}

template <typename Key, typename Value, class Less>
bool listmap<Key,Value,Less>::empty () const {
   return head == NULL;
}


template <typename Key, typename Value, class Less>
xpair<Key,Value>& listmap<Key,Value,Less>::iterator::operator* () {
   TRACE ('l', where->pair);
   return where->pair;
}

template <typename Key, typename Value, class Less>
xpair<Key,Value> *listmap<Key,Value,Less>::iterator::operator-> () {
   TRACE ('l', where->pair);
   return &(where->pair);
}

template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator++ () {
   TRACE ('l', "First: " << map << ", " << where);
   TRACE ('l', "Second: " << map->head << ", " << map->tail);
   where = where->next;
   return *this;
}

template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator-- () {
   where = where->prev;
   return *this;
}

template <typename Key, typename Value, class Less>
bool listmap<Key,Value,Less>::iterator::operator==
            (const iterator& that) const {
   return this->where == that.where;
}

template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::iterator::iterator (listmap *map,
            node *where): map (map), where (where){
}

template <typename Key, typename Value, class Less>
void listmap<Key,Value,Less>::iterator::erase () {
   TRACE ('l', "map = " << map << ", where = " << where << endl);
   node* head = map->head;
   node* tail = map->tail;
   if(where == head and where == tail) {
      delete where;
   } else if ( where == head) {
      map->head = head->next;
      delete where;
   } else if ( where == tail) {
      map->tail = tail->prev;
      delete where;
   } else {
      where->prev->next = where->next;
      where->next->prev = where->prev;
      delete where;
   } 
}
