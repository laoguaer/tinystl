#ifndef HEAP_H
#define HEAP_H
#include "vector.h"
#include "iterator.h"
namespace mystl {

template <class Iterator, 
        class Distance = typename iterator_traits<Iterator>::difference_type,
        class Compare>
void make_heap(Iterator first, Iterator last, Compare cmp) {
    auto len = last - first;
    auto holeIdx = len / 2;
    if (len < 2) return;

    while (holeIdx) {
        down_heap(first, holeIdx, len, *(first + holeIdx), cmp);
        --holeIdx;
    }
}

template <class Iterator
        , class Distance
        , class T
        , class Compare>
void down_heap(Iterator first, Distance holeIdx, Distance len, const T& value, Compare cmp) {
    auto lchild = holeIdx * 2;
    auto rchild = holeIdx * 2 + 1;
    auto next = holeIdx;
    if (lchild <= len && *(first + holeIdx) < *(first + lchild)) {
        next = lchild;
    }
    if (rchild <= len && *(first + next) < *(first + rchild)) {
        next = rchild;
    }

    if (next != holeIdx) {
        *(first + holeIdx) = *(first + next);
        down_heap(first, next, len, value, cmp);
    }
    else {
        *(first + holeIdx) = value;
    }
}

template <class Iterator
        , class Distance
        , class T
        , class Compare>
void up_heap(Iterator first, Distance holeIdx, const T& value, Compare cmp) {
    Distance parent = holeIdx / 2;   
    if (parent) {
        if (value > *(first + parent)) {
            *(first + holeIdx) = *(first + parent);
            up_heap(first, parent, value, cmp);
        }
        else {
            *(first + holeIdx) = value;
        }
    }   
    else {
        if (value > *(first + holeIdx)) {
            *(first + holeIdx) = value;
        }
    }
}

template <class Iterator, class Compare>
void pop_heap(Iterator first, Iterator last, typename iterator_traits<Iterator>::const_reference value, Compare cmp) {
    down_heap(first, static_cast<ptrdiff_t>(1), last - first - 1, value, cmp);
}

template <class Iterator, class Compare, class T>
void push_heap(Iterator first, Iterator last, const T& value, Compare cmp) {
    up_heap(first, last - first - 1, value, cmp);
}











}
#endif