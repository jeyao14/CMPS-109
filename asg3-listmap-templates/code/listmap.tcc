// $Id: listmap.tcc,v 1.15 2019-10-30 12:44:53-07 - - $
// Jeffrey Yao jeyao
// Herman Wu hwwu

#include "listmap.h"
#include "debug.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename key_t, typename mapped_t, class less_t>
listmap<key_t,mapped_t,less_t>::~listmap() {
   DEBUGF ('l', reinterpret_cast<const void*> (this));
   if(empty()){
   }
   else{
      for(auto iter = begin(); iter != end();){
         auto to_erase = iter;
         ++iter;
         to_erase.erase();
         delete to_erase.where;
      }
   }
}

//
// iterator listmap::insert (const value_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::insert (const value_type& pair) {
   DEBUGF ('i', &pair << "->" << pair);

   node* my_node;

   // if the list only contains the anchor
   if(empty()){
      DEBUGF ('i', "list is empty and inserting");

      my_node = new node(anchor(),anchor(),pair);

      anchor()->next = my_node;
      anchor()->prev = my_node;

      return begin();
   }

   DEBUGF ('i', "beginning to iterate");

   for(iterator iter = begin(); iter != end(); ++iter){
      node* searched_node = iter.where;

      DEBUGF ('i', "comparing first less");
      // if inserted node is less than searched node
      if(less(pair.first, searched_node->value.first)){
        DEBUGF ('i', "inserted node is less " << 
            "than searched node and is being inserted");
         // Insert node before searched node
         // Obtain appropriate references
         node* prev_node = searched_node->prev;

         my_node = new node(searched_node,prev_node,pair);

         prev_node->next = my_node;
         searched_node->prev = my_node;

         return iter;
      }
      else{
         DEBUGF ('i', "inserted node is not less than searched node");
         // if inserted node is not greater than searched node
         // AKA if inserted node is equal to searched node
         // AKA replace existing node
         if(!less(searched_node->value.first, pair.first)){
            DEBUGF ('i', "inserted node is " << 
                "equal and is being inserted");
            // replace on the searched node
            // Obtain appropriate references

            auto next_iter = erase(searched_node);

            node* next_node = next_iter.where;
            node* prev_node = next_node->prev;

            my_node = new node(next_node,prev_node,pair);

            prev_node->next = my_node;
            next_node->prev = my_node;

            return --next_iter;
         }
         // if it is greater, keep searching
      }
      DEBUGF ('i', "end of loop round");
   }
   DEBUGF ('i', "end of for loop");

   node* next_node = end().where;
   node* prev_node = end().where->prev;

   my_node = new node(next_node,prev_node,pair);

   prev_node->next = my_node;
   next_node->prev = my_node;

   return end().where->prev;
}

//
// listmap::find(const key_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::find (const key_type& that) {
   DEBUGF ('l', that);

   for(iterator iter = begin(); iter != end(); ++iter){
        bool is_less = less(iter.where->value.first, that);
        bool is_greater = less(that, iter.where->value.first);

        if(!is_less and !is_greater){
            return iter;
        }
    }      

   // If the key is not found, return the anchor
   return end();
}

//
// iterator listmap::erase (iterator position)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::erase (iterator position) {
    DEBUGF ('l', &*position);
    //check if there is something to remove
    if(position != end()){
      
        iterator iter(position.where->next);
        position.erase();
        delete position.where;

        return iter;
    }else{
   // if the key doesn't exist
       return end();
    }
}

template <typename key_t, typename mapped_t, class less_t>
void listmap<key_t,mapped_t,less_t>::iterator::erase () {
   node* next_node = where->next;
   node* prev_node = where->prev;

   next_node->prev = prev_node;
   prev_node->next = next_node;
}
