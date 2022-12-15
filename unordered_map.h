#ifndef UNORDERED_MAP
#define UNORDERED_MAP
#include "hashtable.h"

namespace mystl {

template <class Key, class Value, class Hash = hash<Key>, class KeyEqual = equal_to<Key>>
class unordered_map {
public:
    typedef pair<Key, Value>                                                value_type;
    typedef Key                                                             key_type;
    typedef hash_table<Key, value_type, select1st<value_type>, KeyEqual, Hash>      container;
    typedef hash_iterator<Key, value_type, select1st<value_type>, KeyEqual, Hash>   iterator;
    typedef typename iterator::pointer                                      pointer;
    typedef typename iterator::reference                                    reference;
    typedef typename iterator::const_reference                              const_reference;
    typedef typename iterator::difference_type                              difference_type;
    typedef typename iterator::size_type                                    size_type;


public:
    size_type size() { return c.size(); }
    size_type capacity() { return c.bucket_size; }

    iterator begin() { return c.begin(); }
    iterator end() { return c.end(); }
    

    pair<iterator, bool> insert(const value_type& val) { return c.insert_unique(val); }
    
    iterator find(const key_type& key) { return c.find(key); }
    
private:
    container c;
};

}






#endif