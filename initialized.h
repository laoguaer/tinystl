
#ifndef INTITIALIZED_H
#define INTITIALIZED_H
#include "construct.h"
#include <cstddef>


namespace mystl {
//copy first-last to res
template<class InputIt, class ForwardIt>
ForwardIt initialize_copy(InputIt first, InputIt last, ForwardIt res) {
    for (; first != last; first++) {
        construct(&*res++, forward<typename iterator_traits<ForwardIt>:: value_type>(*first));
    }
    return res;
}

template<class InputIt, class ForwardIt>
ForwardIt initialize_copy_r(InputIt first, InputIt last, ForwardIt res) {
    if (last == first) return res;
    int cnt = last - first;
    ForwardIt ret = res + cnt;
    res += cnt - 1;
    --last;
    for (int i = 0; i < cnt; i++) {
        construct(&*res--, forward<typename iterator_traits<ForwardIt>:: value_type>(*last--));
    }
    return ret;
}

//此处参数能萃取类型吗????
template<class InputIt, class T>
void initialize_fill(InputIt first, InputIt last, T&& x) 
{
    for (; first != last; ++first) {
        mystl::construct(&*first, forward<T>(x));
    }
}

//从first初始化n个x
template<class ForwardIt, class T>
void initialize_fill_n(ForwardIt first, size_t n, T&& x) {
    for (size_t i = 0; i < n; ++i, ++first) {
        mystl::construct(&*first, forward<T>(x));
    }
}
    
}

#endif