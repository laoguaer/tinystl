#ifndef SET_H
#define SET_H

#include "rb_tree.h"

namespace mystl {

template <class value_type, class Compare = less<value_type>> 
class set {
private:
  // 以 mystl::rb_tree 作为底层机制
    typedef mystl::rb_tree<value_type, value_type, identity<value_type>, Compare>  base_type;
    base_type m_tree;

public:
  // 使用 rb_tree 定义的型别
    typedef typename base_type::iterator                iterator;
    typedef typename base_type::difference_type         difference_type;
    typedef typename base_type::size_type               size_type;
    typedef typename base_type::reference               reference;
    typedef typename base_type::const_reference         const_reference;
    
    typedef value_type                                  key_type;
    typedef Compare                                     key_compare;

public:
    // 插入删除
    pair<iterator, bool> insert(const value_type& value)
    {
        return m_tree.insert_unique(value);
    }
    iterator erase(iterator it) { return m_tree.erase(it); }

    //查找
    iterator find(const value_type& val) { return m_tree.find(val); }

    void for_each() { m_tree.for_each(m_tree.get_header()->parent); }
    

    // 容量函数
    size_type size() const { return m_tree.size(); }

    iterator begin() { return m_tree.begin(); }
    iterator end() { return m_tree.end(); }
};



}
#endif