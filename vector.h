#ifndef VECTOR_H
#define VECTOR_H

#include "allocator.h"   
#include "initialized.h"
#include "iterator.h"
#include <assert.h>
#include "functional.h"
#include "algo.h"
#include "util.h"
namespace mystl {
    

template<class T, class Alloc = alloc<T>> 
class vector {
public:
	typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
    typedef T* iterator;

public:
    vector() : _finish(nullptr), _first(nullptr), _end_store(nullptr) {};
    ~vector() { 
        mystl::destory(_first, _finish); 
        _deallocate();
        _finish = _first = _end_store = nullptr;
    };
    vector(size_type n, T&& x) {
        _fill_allocate(n, forward<T>(x));
    }
    vector(size_type n) {
        _fill_allocate(n, value_type());
    }
    vector(vector&& rhs) : _first(rhs._first), _finish(rhs._finish), _end_store(rhs._end_store) {
        rhs._first = nullptr;
        rhs._finish = nullptr;
        rhs._end_store = nullptr;
    }
    vector(vector& rhs) {
        _fill_allocate(rhs.size(), value_type());
        initialize_copy(rhs._first, rhs._finish, _first);
    }

    size_type size() const { return _finish - _first; }
    size_type capacity() const { return _end_store - _first; }
    bool empty() const {return _first == _finish; }
    void reserve(size_type n);

    iterator begin() { return _first; }
    iterator end() { return _finish; } 

    reference front() { return *_first; }
    reference back() { return *(_finish - 1); }
    void push_back(T&& x);
    void push_back(T& x);
    void pop_back();

    template<class... Args>
    void emplace_back(Args &&...args);

    //如果在begin()插入相当于push_front();
    iterator insert(iterator it, const T& x);
    iterator insert(iterator it, size_type n, T& x);
    iterator insert(iterator it, size_type n, T&& x);

	// swap
	void swap(vector &rhs) noexcept;

    
    // opeartor
    reference operator[](int n) {
        assert(n < size());
        return *(_first + n);
    }
    const_reference operator[] (int n) const {
        assert(n < size());
        return *(_first + n);
    }
    reference operator= (const_reference rhs);

private:
    // 调用分配器的deallocate() 调配内存到内存池
    void _deallocate() { Alloc::deallocate(_first, capacity()); }
    
    //初始化n个x; 
    void _fill_allocate(int n, T&& x);

    //扩大 复制 初始化
    void _double_copy();

private:
    iterator _first;
    iterator _finish;
    iterator _end_store;
};


template<class T, class Alloc>
void vector<T, Alloc>::_fill_allocate(int n, T&& x) { 
        iterator p = Alloc::allocate(n); 
        mystl::initialize_fill_n(p, n, forward<T>(x));
        _first = p;
        _finish = p + n;
        _end_store = _finish;
}


template<class T, class Alloc>
void vector<T, Alloc>::_double_copy() { 
    int n = size();
    if (n == 0) {
        _first = Alloc::allocate(1);
        _finish = _first;
        _end_store = _first + 1;
    }

    iterator old_first = _first;
    iterator old_end = _finish;

    // 扩容
    _first = Alloc::allocate(2 * n);
    _end_store = _first + 2 * n;

    // 复制
    _finish = initialize_copy(old_first, old_end, _first);

    //销毁旧的内存
    destory(old_first, old_end);
    _deallocate();
}

template<class T, class Alloc>
typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(iterator it, const T& x)  {
    return insert(it, 1, x);
}

template<class T, class Alloc>
typename vector<T, Alloc>::iterator vector<T, Alloc>::
  insert(iterator it, size_type n, T& x) {
    assert(n > 0);
    if (capacity() < size() + n) {

        size_type off = it - _first;

        if (capacity() == 0) {
            _finish = _first = Alloc::allocate(1);
            _end_store = _finish + 1;
            return insert(end(), n, x);
        }

        int cnt = capacity();
        while (cnt < size() + n) {
            cnt <<= 1;
        }

        iterator new_first = Alloc::allocate(cnt);
        iterator new_finish = initialize_copy(_first, _finish, new_first);

        destory(_first, _finish);
        _deallocate();

        _first = new_first;
        _finish = new_finish;
        _end_store = _first + cnt;


        it = _first + off;
        return insert(it, n, x);
    }
    
    size_type elem_after = _finish - it;

    if (elem_after <= n) {
        initialize_copy(it, _finish, it + n);
        initialize_fill_n(it, n, x);
        _finish += n;
    }
    else {
        initialize_copy_r(it, _finish, it + n);
        initialize_fill_n(it, n, x);
        _finish += n;
    }
    return it;
}

template<class T, class Alloc>
typename vector<T, Alloc>::iterator vector<T, Alloc>::
  insert(iterator it, size_type n, T&& x) {
    assert(n > 0);
    if (capacity() < size() + n) {

        size_type off = it - _first;

        if (capacity() == 0) {
            _finish = _first = Alloc::allocate(1);
            _end_store = _finish + 1;
            return insert(end(), n, move(x));
        }

        int cnt = capacity();
        while (cnt < size() + n) {
            cnt <<= 1;
        }

        iterator new_first = Alloc::allocate(cnt);
        iterator new_finish = initialize_copy(_first, _finish, new_first);

        destory(_first, _finish);
        _deallocate();

        _first = new_first;
        _finish = new_finish;
        _end_store = _first + cnt;


        it = _first + off;
        return insert(it, n, move(x));
    }
    
    size_type elem_after = _finish - it;

    if (elem_after <= n) {
        initialize_copy(it, _finish, it + n);
        initialize_fill_n(it, n, move(x));
        _finish += n;
    }
    else {
        initialize_copy_r(it, _finish, it + n);
        initialize_fill_n(it, n, move(x));
        _finish += n;
    }
    return it;
}

template<class T, class Alloc>
void vector<T, Alloc>::push_back(T& x) {
    insert(end(), 1u, x);
}

template<class T, class Alloc>
void vector<T, Alloc>::push_back(T&& x) {
    insert(end(), 1u, move(x));
}


template<class T, class Alloc>
void vector<T, Alloc>::pop_back() {
    assert(_finish != _first);
    destory(--_finish);
}

// 在尾部就地构造元素，避免额外的复制或移动开销
// template <class T, class Alloc>
// template <class... Args>
// void vector<T, Alloc>::emplace_back(Args &&...args)
// {
// 	if (_finish != _end_store)
// 	{
// 		data_allocator::construct(_finish, mystl::forward<Args>(args)...);
// 		++_finish;
// 	}
// 	else
// 	{
// 		reallocate_emplace(end_, mystl::forward<Args>(args)...);
// 	}
// }

template<class T, class Alloc>
void vector<T, Alloc>::reserve(size_type n) 
{
    if (n > capacity()) {
        size_type old_size = size();
        auto temp = Alloc::allocate(n);
        initialize_copy(_first, _finish, temp);
        destory(_first, _finish);
        Alloc::deallocate(&*(_first), _finish - _first);
        _first = temp;
        _finish = temp + n;
    }
}

template <class T, class Alloc>
typename vector<T, Alloc>::reference vector<T, Alloc>::operator= (const_reference rhs) {
    // if (this != &rhs) {
    //     const size_type n = rhs.size();
    //     if (capacity() >= n) {
    //         initialize_copy(rhs.begin(), rhs.end(), begin());
    //         if (size() > n) {
    //             Alloc::deallocate(&*(end() - (size() - n)), size() - n);
    //         }
    //         _finish = _first + n;
    //     }
    //     else {
    //         ~vector();
    //         // resize --------------------

    //         _first = Alloc::allocate(n);
    //         _finish = _first + n;
    //         _end_store = _finish;
    //         initialize_copy(rhs.begin(), rhs.end(), _first);
    //     }
    // }
    return *this;
}


// 与另一个 vector 交换
template <class T, class Alloc>
void vector<T, Alloc>::swap (vector<T, Alloc> &rhs) noexcept
{
	if (this != &rhs)
	{
		mystl::swap(_first, rhs._first);
		mystl::swap(_finish, rhs._finish);
		mystl::swap(_end_store, rhs._end_store);
        printf("swap:  first : %p, %p \n", this->_first, rhs._first);
	}
}

// 重载 mystl 的 swap
template <class T>
void swap(vector<T> &lhs, vector<T> &rhs)
{
	lhs.swap(rhs);
}

}
#endif