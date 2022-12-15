#ifndef ITERATOR_H
#define ITERATOR_H
#include <cstddef>
#include <type_traits>

namespace mystl {


// 五种迭代器类型
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

// iterator 模板  自行设计的iterator需继承此父类
template <class Category, class T, class Distance = ptrdiff_t,
    class Pointer = T*, class Reference = T&>
struct iterator {
    typedef Category                             iterator_category;
    typedef T                                    value_type;
    typedef Pointer                              pointer;
    typedef Reference                            reference;
    typedef Distance                             difference_type;
    typedef const T&                             const_reference;
    typedef size_t                               size_type;
};

template <class Iterator>
struct iterator_traits {
    typedef typename Iterator::iterator_category iterator_category;
    typedef typename Iterator::value_type        value_type;
    typedef typename Iterator::pointer           pointer;
    typedef typename Iterator::reference         reference;
    typedef typename Iterator::const_reference   const_reference;
    typedef typename Iterator::difference_type   difference_type;
};

template <class T>
struct iterator_traits<T*> {
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef ptrdiff_t difference_type;
    typedef const T& const_reference;
};



// 萃取某个迭代器的 category
template <class Iterator>
typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&) {
    typedef typename iterator_traits<Iterator>::iterator_category category;
    return category();
}

// 萃取某个迭代器的 distance_type
template <class Iterator>
typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&) {
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

// 萃取某个迭代器的 value_type
template <class Iterator>
typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&) {
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}



// 以下函数用于计算迭代器间的距离

// distance 的 input_iterator_tag 的版本
template <class InputIterator>
typename iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last, input_iterator_tag)
{
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last)
    {
        ++first;
        ++n;
    }
    return n;
}

// distance 的 random_access_iterator_tag 的版本
template <class RandomIter>
typename iterator_traits<RandomIter>::difference_type
__distance(RandomIter first, RandomIter last,
                  random_access_iterator_tag)
{
    return last - first;
}

template <class InputIterator>
typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last)
{
    return __distance(first, last, iterator_category(first));
}



// 以下函数用于让迭代器前进 n 个距离

// advance 的 input_iterator_tag 的版本
template <class InputIterator, class Distance>
void __advance(InputIterator& i, Distance n, input_iterator_tag)
{
    while (n--) 
        ++i;
}

// advance 的 bidirectional_iterator_tag 的版本
template <class BidirectionalIterator, class Distance>
void __advance(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag)
{
    if (n >= 0)
        while (n--)  ++i;
    else
        while (n++)  --i;
}

// advance 的 random_access_iterator_tag 的版本
template <class RandomIter, class Distance>
void __advance(RandomIter& i, Distance n, random_access_iterator_tag)
{
    i += n;
}

template <class InputIterator, class Distance>
void advance(InputIterator& it, Distance n)
{
    __advance(it, n, iterator_category(it));
}







}
#endif