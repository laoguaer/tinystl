#ifndef CONSTRUCT_H
#define CONSTRUCT_H
#include <new>
#include "util.h"
// 对象在已有内存的构造和析构

namespace mystl{

//全局construct  调用placement new
template<class Pointer, class T>
inline void construct(Pointer p, T&& x) {
    new(p) T(move(x));
}

template<class Pointer, class T>
inline void construct(Pointer p, T& x) {
    new(p) T(x);
}



//全局destroy 调用p->~T();
template<class T>
inline void destory(T* p) {
    p->~T();
}

//destroy 传入两个迭代器 判断trivial
template<class T>
inline void destory(T* first, T* last) {
    for(;first != last; ++first) {
        first->~T();
    }
}

template <class ForwardIt>
inline void destory(ForwardIt first, ForwardIt last) {
    for (; first != last; ++first) {
        destory(&*first);
    }
}
}

#endif