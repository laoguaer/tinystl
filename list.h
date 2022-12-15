#ifndef LIST_H
#define LIST_H

#include "iterator.h"
#include "allocator.h"
#include "initialized.h"
#include <assert.h>
namespace mystl {

template <class T>
struct __list_node {
    typedef __list_node* pointer;
    pointer prev;
    pointer next;
    T data;
};


// list迭代器设计
template <class T>
struct list_iterator : public iterator<bidirectional_iterator_tag, T> {
    typedef __list_node<T>*                     link_type;
    typedef list_iterator                       self;

    typedef typename self::iterator_category            iterator_category;
    typedef typename self::value_type                   value_type;
    typedef typename self::pointer                      pointer;
    typedef typename self::reference                    reference;
    typedef typename self::const_reference              const_reference;
    typedef typename self::difference_type              difference_type;

    link_type node;

    // 构造函数
    list_iterator(const link_type& x) : node(x) {}
    list_iterator(const list_iterator& rhs) : node(rhs.node) {}

    bool operator== (const self& rhs) const { return node == rhs.node; }
    bool operator!= (const self& rhs) const { return node != rhs.node; }

    reference operator* () const { return node->data; }
    pointer operator-> () const { return &operator*(); }

    self& operator++ () {
        node = node->next;
        return *this;
    } 
    self operator++ (int) {
        self temp = *this;
        node = node->next;
        return temp;
    }
    self& operator-- () {
        node = node->prev;
        return *this;
    } 
    self operator-- (int) {
        self temp = *this;
        node = node->prev;
        return temp;
    }
};


template <class T, class Alloc = alloc<__list_node<T>>>
class list {
public:
    typedef T                       value_type;
	typedef T*                      pointer;
	typedef const T*                const_pointer;
	typedef T&                      reference;
	typedef const T&                const_reference;
	typedef size_t                  size_type;
	typedef ptrdiff_t               difference_type;

    typedef list_iterator<T>        iterator;
    typedef __list_node<T>          list_node;
    typedef list_node*               link_type;

public:

    // 构造函数
    list() {
        init();
    }


    iterator begin() { return node->next; }
    iterator end() { return node; }
    bool empty() { return node->next == node; }
    size_type size() { return distance(begin(), end()); }
    reference front() { return *begin(); }
    reference back() { return *(--end()); }

    iterator insert(iterator it, const_reference x);
    iterator erase(iterator it);
    void push_back(const_reference x) { insert(end(), x); }
    void push_front(const_reference x) { insert(begin(), x); }
    void pop_front() { erase(begin()); }
    void pop_back() { erase(--end()); }

protected:
    link_type get_node() {
        return static_cast<link_type>(Alloc::allocate());
    }
    void put_node(link_type p) {
        Alloc::deallocate(p);
    }
    void construct_node(link_type p) {
        construct(p);
    }
    void destory_node(link_type p) {
        destory(p);
    }

    void init() {
        node = get_node();
        construct_node(node);
        node->next = node;
        node->prev = node;
    }

private:
    link_type create_one_node(const_reference x) {
        link_type ret = get_node();
        construct(&ret->data, x);
        return ret;
    }

private:
    link_type node;  
};


template<class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::insert(iterator it, const_reference x) {
    link_type temp = create_one_node(x);
    temp->next = it.node;
    temp->prev = it.node->prev;
    (it.node)->prev->next = temp;
    (it.node)->prev = temp;
    return temp;
}

template<class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::erase(iterator it) {
    assert(it.node != node);
    link_type temp = it.node;
    link_type ret = temp->prev;
    temp->prev->next = temp->next;
    temp->next->prev = temp->prev;
    destory_node(temp);
    put_node(temp);
    return ret;
}






















}
#endif