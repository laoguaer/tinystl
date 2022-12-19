#ifndef STACK_H
#define STACK_H
#include "deque.h"

namespace mystl {

template <class T, class Container = mystl::deque<int>>
class stack {
public:
    typedef Container                           container_type;
    // 使用底层容器的型别
    typedef typename Container::value_type      value_type;
    typedef typename Container::size_type       size_type;
    typedef typename Container::reference       reference;
    typedef typename Container::const_reference const_reference;

public:
    stack() = default;
    stack(size_type n) : c(n) {}
    stack(size_type n, const_reference x) : c(n, x) {}
    template <class Iterator>
    stack(Iterator first, Iterator last) : c(first, last) {}

public: 
    size_type size() { return c.size(); }
    const_reference top() { return c.back(); }
    void pop() { return c.pop_back(); }
    void push(const_reference x) { c.push_back(x); }

private:
    container_type c; 
};


}


#endif