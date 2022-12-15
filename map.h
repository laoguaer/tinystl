#ifndef MAP_H
#define MAP_H

#include "rb_tree.h"

namespace mystl {

template <class Key, class Value, class Compare = less<Key>>
class map {
public:
    typedef Key                                         key_type;   
    typedef pair<key_type, Value>                       value_type;
    typedef Compare                                     key_compare;
private:
  // 以 mystl::rb_tree 作为底层机制
    typedef mystl::rb_tree<Key, value_type, select1st<value_type>, key_compare>  base_type;
    base_type m_tree;

public:
  // 使用 rb_tree 定义的型别
    typedef typename base_type::iterator                iterator;
    typedef typename base_type::difference_type         difference_type;
    typedef typename base_type::size_type               size_type;
    typedef typename base_type::reference               reference;
    typedef typename base_type::const_reference         const_reference;
    
public:
    pair<iterator, bool> insert(const value_type& val) { return m_tree.insert_unique(val); }
    iterator erase(const iterator& it) { return m_tree.erase(it); }

    iterator begin() { return m_tree.begin(); }
    iterator end() { return m_tree.end(); }

    iterator find(const key_type& key) { return m_tree.find(key); }
    void for_each() { m_tree.for_each(m_tree.get_header()->parent); }

    size_type size() { return m_tree.size(); }
};
}

#endif  