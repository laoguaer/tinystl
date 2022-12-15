#ifndef ALLOC_H
#define ALLOC_H

#include <cstddef>
namespace mystl {

template<class T>
class alloc {
public:

    static T* allocate(size_t n = 1);
    static void deallocate(T*, size_t n = 1);

    template <class ForwardIt>
    static void deallocate(ForwardIt start, size_t n = 1);

    static void construct(T*, const T&);
    static void destroy(T*);
private:
    size_t upward(size_t n);
};




template<class T>
T* alloc<T>::allocate(size_t n) {
    if (n == 0)  return nullptr;
    size_t _size = (sizeof T()) * n;
    if (sizeof T() < 1024) {
        return static_cast<T*>(::operator new(_size));
    }
}

// deallocate
template<class T>
void alloc<T>::deallocate(T* ptr, size_t n) {
    if (n == 0)  return;
    size_t _size = (sizeof T()) * n;
    if (sizeof T() < 1024) {
    }
    ::operator delete(ptr);
}

template <class T>
template<class ForwardIt>
void alloc<T>::deallocate(ForwardIt start, size_t n) {
    if (n == 0) return;
    while (n --) {
        deallocate(&*(start++));
    }
}


template<class T>
inline size_t alloc<T>::upward(size_t n) {
    return n % 8 == 0 ? n / 8 : (n / 8) + 1;
}


};

#endif