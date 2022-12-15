#ifndef QUEUE_H
#define QUEUE_H
#include "deque.h"
#include "vector.h"
#include "heap.h"
#include "algo.h"
#include "functional.h"
namespace mystl {

template <class T, class Container = deque<T>>
class queue {
public:
    typedef Container                           container_type;
    // 使用底层容器的型别
    typedef typename Container::value_type      value_type;
    typedef typename Container::size_type       size_type;
    typedef typename Container::reference       reference;
    typedef typename Container::const_reference const_reference;
public:
    size_type size() { return c.size(); }
    void push(const_reference x) { c.push_back(x); }
    void pop() { c.pop_front(); }
    reference front() { return c.front(); }


private:
    container_type c;
};






template <class T, class Container = vector<int>, class Compare = mystl::less<T>>
class priority_queue {
public:
    typedef Container                           container_type;
    typedef Compare                             compare_type;
    // 使用底层容器的型别
    typedef typename Container::value_type      value_type;
    typedef typename Container::size_type       size_type;
    typedef typename Container::reference       reference;
    typedef typename Container::const_reference const_reference;

public:
    priority_queue(size_type n = 0) : c(n + 1) {} 
    priority_queue(size_type n, const_reference value) : c(n + 1, value) {}

public:
    size_type size() { return c.size() - 1; }
    bool empty() { return size() == 0; }

    void push(const_reference x) {
        c.push_back(x); 
        push_heap(c.begin(), c.end(), x, cmp);
    }
    void pop() { 
        assert(size() > 0);
        const_reference value = c.back();
        top() = c.back();
        c.pop_back(); 

        pop_heap(c.begin(), c.end(), value, cmp);  
    }

    reference top() { assert(size() > 0); return c[1]; }

    container_type c;
private:
    compare_type cmp;
};



}



#endif