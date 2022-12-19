#ifndef UNORDERED_SET_H
#define UNORDERED_SET_H
#include "hashtable.h"
namespace mystl {

template <class Key, class Hash = hash<Key>, class KeyEqual = equal_to<Key>>
class unordered_set {
public:     
    typedef hash_table<Key, Key, identity<Key>, KeyEqual, Hash>     container;
    typedef hash_iterator<Key, Key, identity<Key>, KeyEqual, Hash>  iterator;
    typedef typename iterator::pointer                              pointer;
    typedef typename iterator::reference                            reference;
    typedef typename iterator::const_reference                      const_reference;
    typedef typename iterator::difference_type                      difference_type;
    typedef typename iterator::size_type                            size_type;
    typedef Key                                                     key_type;

public:
    size_type size() { return c.size(); }
    size_type capacity() { return c.bucket_size; }

    iterator begin() { return c.begin(); }
    iterator end() { return c.end(); }
    

    pair<iterator, bool> insert(const key_type& val) { return c.insert_unique(val); }
    
    iterator find(const key_type& key) { return c.find(key); }
    
private:
    container c;
};







}
#endif