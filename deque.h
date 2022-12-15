#ifndef DEQUE_H
#define DEQUE_H
#include "iterator.h"
#include "initialized.h"
#include <initializer_list>
#include "algo.h"
namespace mystl{
#define DEQUE_MAP_INIT_SIZE 8
// buffer__size
template <class T>
struct deq_buff_size {
    static constexpr size_t value = sizeof(T) < 256 ? 4096 / sizeof(T) : 16;
};

// 迭代器设计
template <class T>
struct deque_iterator : public mystl::iterator<random_access_iterator_tag, T> {
    typedef deque_iterator<T>                               iterator;
    typedef deque_iterator                                  self;
    typedef typename iterator::iterator_category            iterator_category;
    typedef typename iterator::value_type                   value_type;
    typedef typename iterator::pointer                      pointer;
    typedef typename iterator::reference                    reference;
    typedef typename iterator::difference_type              difference_type;
    typedef pointer*                                        map_pointer;

//  此处需要重新设置
    static constexpr size_t buffer_size = deq_buff_size<T>::value;

    // 迭代器所含成员数据
    pointer cur;    // 指向所在缓冲区的当前元素
    pointer first;  // 指向所在缓冲区的头部
    pointer last;   // 指向所在缓冲区的尾部
    map_pointer   node;   // 缓冲区所在节点

        // 构造、复制、移动函数
    deque_iterator() : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}

    deque_iterator(const iterator& rhs)
        :cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}

    deque_iterator(iterator&& rhs) noexcept
        :cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node)
    {
        rhs.cur = nullptr;
        rhs.first = nullptr;
        rhs.last = nullptr;
        rhs.node = nullptr;
    }

    deque_iterator(pointer v, map_pointer n)
    : cur(v), first(*n), last(*n + buffer_size), node(n) {}

    reference operator* () const { return *cur; }
    pointer operator-> () const { return cur; }

    deque_iterator& operator=(const iterator& rhs) {
        if (this != &rhs)
        {
        cur = rhs.cur;
        first = rhs.first;
        last = rhs.last;
        node = rhs.node;
        }
        return *this;
    }


    // 转到另一个缓冲区
    void set_node(map_pointer new_node)
    {
        node = new_node;
        first = *new_node;
        last = first + buffer_size;
    }


    self& operator++() {
        ++cur;
        if (cur == last)
        { // 如果到达缓冲区的尾
        set_node(node + 1);
        cur = first;
        }
        return *this;
    }
    self operator++(int)
    {
        self tmp = *this;
        ++*this;
        return tmp;
    }

    self& operator--()
    {
        if (cur == first)
        { // 如果到达缓冲区的头
        set_node(node - 1);
        cur = last;
        }
        --cur;
        return *this;
    }
    self operator--(int)
    {
        self tmp = *this;
        --*this;
        return tmp;
    }
    // 就算距离
    difference_type operator- (const self& rhs) {
        return static_cast<difference_type>( buffer_size * (node - rhs.node) + 
                (cur - first) - (rhs.cur - rhs.first));
    }

    // 加减迭代器
    self& operator+=(difference_type n)
    {
        const auto offset = n + (cur - first);
        if (offset >= 0 && offset < static_cast<difference_type>(buffer_size))
        { // 仍在当前缓冲区
            cur += n;
        }
        else
        { // 要跳到其他的缓冲区
            const auto node_offset = offset > 0
                ? offset / static_cast<difference_type>(buffer_size)
                : -static_cast<difference_type>((-offset - 1) / buffer_size) - 1;
            set_node(node + node_offset);
            cur = first + (offset - node_offset * static_cast<difference_type>(buffer_size));
        }
        return *this;
    }

    self& operator-= (difference_type n) {
        return *this += -n;
    }

    self operator- (difference_type n) {
        self temp = *this;
        temp -= n;
        return temp;
    }
    self operator+ (difference_type n) {
        self temp = *this;
        temp += n;
        return temp;
    }

    reference operator[] (difference_type n) {
        return *(*this + n);
    }
        // 重载比较操作符
    bool operator==(const self& rhs) const { return cur == rhs.cur; }
    bool operator< (const self& rhs) const
        { return node == rhs.node ? (cur < rhs.cur) : (node < rhs.node); }
    bool operator!=(const self& rhs) const { return !(*this == rhs); }
    bool operator> (const self& rhs) const { return rhs < *this; }
    bool operator<=(const self& rhs) const { return !(rhs < *this); }
    bool operator>=(const self& rhs) const { return !(*this < rhs); }
};


template <class T>
class deque {
public:
    typedef T                               value_type;
	typedef T*                              pointer;
	typedef const T*                        const_pointer;
	typedef T&                              reference;
	typedef const T&                        const_reference;
	typedef size_t                          size_type;
	typedef ptrdiff_t                       difference_type;
    typedef deque_iterator<T>               iterator;
    typedef pointer*                        map_pointer;

    typedef alloc<T>                        alloc_data;
    typedef alloc<T*>                       alloc_map;
protected:
    iterator m_start;
    iterator m_finish;
    map_pointer m_map;
    size_type m_size_map;
public:
    iterator begin() { return m_start; }
    iterator end() { return m_finish; }
    size_type size() { return m_finish - m_start; }
    
    // 构造函数
    deque(size_type n = 0) {
        fill_init(n, value_type());
    }
    deque(size_type n, const_reference x) {
        fill_init(n, x);
    }

    // ------------------------------------- -?？？？?？?？?？?？?？?？?？?？?？?？?？?？?？?？?？?、??
    // 会乱推导到此函数
    template <class Iterator, class size_type = typename Iterator::size_type>
    deque(Iterator first, Iterator second) {
        copy_init(first, second);
    }

    //front back
    reference front() { return *begin(); }
    reference back() { return *(end() - 1); } 

    //push
    void push_back(const_reference x);
    void push_front(const_reference x);

    //pop
    void pop_back() { erase(end() - 1); }
    void pop_front() { erase(begin()); }

    //insert
    void insert(iterator pos, const_reference x);
    void insert(iterator pos, size_type n, const_reference x);

    //erase
    iterator erase(iterator start, iterator finish);
    iterator erase(iterator pos) { return erase(pos, pos + 1); }

        // 访问元素相关操作
    reference operator[](size_type n)
    {
        assert(n < size());
        return m_start[n];
    }

private:
    static constexpr size_t buffer_size = deq_buff_size<T>::value;
    void fill_init(size_type n, const_reference x);
    void map_init(size_type n);
    template <class Iterator>
    void copy_init(Iterator first, Iterator last);
    map_pointer create_map(size_type n);
    void create_buffer(map_pointer start, map_pointer finish);
    void require_capacity(size_type n, bool front);
    void reallocate_map_at_front(size_type need_buffer);
    void reallocate_map_at_back(size_type need_buffer);
};

template <class T>
void deque<T>::fill_init(size_type n, const_reference value) {
    map_init(n);
    if (n != 0) {
        for (auto cur = m_start.node; cur < m_finish.node; ++cur)
        {
            mystl::initialize_fill(*cur, *cur + buffer_size, value);
        }
        mystl::initialize_fill(m_finish.first, m_finish.cur, value);
    }
}

template <class T>
template <class FIter>
void deque<T>::
    copy_init(FIter first, FIter last) {
    const size_type n = mystl::distance(first, last);
    map_init(n);
    for (auto cur = m_start.node; cur < m_finish.node; ++cur)
    {
        auto next = first;
        mystl::advance(next, buffer_size);
        mystl::initialize_copy(first, next, *cur);
        first = next;
    }
    mystl::initialize_copy(first, last, m_finish.first);
}

template <class T>
void deque<T>::map_init(size_type nElem) {
    const size_type nNode = nElem / buffer_size + 1; // 需要分配的缓冲区个数
    m_size_map = mystl::max(static_cast<size_type>(DEQUE_MAP_INIT_SIZE), nNode + 2);
    try
    {
        m_map = create_map(m_size_map);
    }
    catch (...)
    {
        m_map = nullptr;
        m_size_map = 0;
        throw;
    }

    // 让 nstart 和 nfinish 都指向 m_map 最中央的区域，方便向头尾扩充
    map_pointer nstart = m_map + (m_size_map - nNode) / 2;
    map_pointer nfinish = nstart + nNode - 1;
    try
    {
        create_buffer(nstart, nfinish);
    }
    catch (...)
    {
        alloc_map::deallocate(m_map, m_size_map);
        m_map = nullptr;
        m_size_map = 0;
        throw;
    }
    m_start.set_node(nstart);
    m_finish.set_node(nfinish);
    m_start.cur = m_start.first;
    m_finish.cur = m_finish.first + (nElem % buffer_size);
}

template <class T>
typename deque<T>::map_pointer deque<T>::create_map(size_type n) { 
    map_pointer mp = nullptr;
    mp = alloc_map::allocate(n);
    for (int i = 0; i < n; ++i) {
        *(mp + i) = nullptr;
    }
    return mp;
}

template <class T>
void deque<T>::create_buffer(map_pointer start, map_pointer finish) { 
    map_pointer cur;
    try
    {
        for (cur = start; cur <= finish; ++cur)
        {
            *cur = alloc_data::allocate(buffer_size);
        }
    }
    catch (...)
    {
        while (cur != start)
        {
            --cur;
            alloc_data::deallocate(*cur, buffer_size);
            *cur = nullptr;
        }
        throw;
    }
}



// 在头部插入元素
template <class T>
void deque<T>::push_front(const value_type &value)
{
    if (m_start.cur != m_start.first)
    {
        construct(m_start.cur - 1, value);
        --m_start.cur;
    }
    else
    {
        require_capacity(1, true);
        try
        {
            --m_start;
            construct(m_start.cur, value);
        }
        catch (...)
        {
            ++m_start;
            throw;
        }
    }
}

// 在尾部插入元素
template <class T>
void deque<T>::push_back(const value_type &value)
{
    if (m_finish.cur != m_finish.last - 1)
    {
        construct(m_finish.cur, value);
        ++m_finish.cur;
    }
    else
    {
        require_capacity(1, false);
        construct(m_finish.cur, value);
        ++m_finish;
    }
}

// require_capacity 函数
template <class T>
void deque<T>::require_capacity(size_type n, bool front)
{
    if (front && (static_cast<size_type>(m_start.cur - m_start.first) < n))
    {
        const size_type need_buffer = (n - (m_start.cur - m_start.first)) / buffer_size + 1;
        if (need_buffer > static_cast<size_type>(m_start.node - m_map))
        {
            reallocate_map_at_front(need_buffer);
            return;
        }
        create_buffer(m_start.node - need_buffer, m_start.node - 1);
    }
    else if (!front && (static_cast<size_type>(m_finish.last - m_finish.cur - 1) < n))
    {
        const size_type need_buffer = (n - (m_finish.last - m_finish.cur - 1)) / buffer_size + 1;
        if (need_buffer > static_cast<size_type>((m_map + m_size_map) - m_finish.node - 1))
        {
            reallocate_map_at_back(need_buffer);
            return;
        }
        create_buffer(m_finish.node + 1, m_finish.node + need_buffer);
    }
}

// reallocate_map_at_front 函数
template <class T>
void deque<T>::reallocate_map_at_front(size_type need_buffer)
{
    const size_type new_map_size = mystl::max(m_size_map << 1,
                                                m_size_map + need_buffer + DEQUE_MAP_INIT_SIZE);
    map_pointer new_map = create_map(new_map_size);
    const size_type old_buffer = m_finish.node - m_start.node + 1;
    const size_type new_buffer = old_buffer + need_buffer;

    // 另新的 map 中的指针指向原来的 buffer，并开辟新的 buffer
    auto begin = new_map + (new_map_size - new_buffer) / 2;
    auto mid = begin + need_buffer;
    auto end = mid + old_buffer;
    create_buffer(begin, mid - 1);
    for (auto begin1 = mid, begin2 = m_start.node; begin1 != end; ++begin1, ++begin2)
        *begin1 = *begin2;

    // 更新数据
    alloc_map::deallocate(m_map, m_size_map);
    m_map = new_map;
    m_size_map = new_map_size;
    m_start = iterator(*mid + (m_start.cur - m_start.first), mid);
    m_finish = iterator(*(end - 1) + (m_finish.cur - m_finish.first), end - 1);
}

// reallocate_map_at_back 函数
template <class T>
void deque<T>::reallocate_map_at_back(size_type need_buffer)
{
    const size_type new_map_size = mystl::max(m_size_map << 1,
                                                m_size_map + need_buffer + DEQUE_MAP_INIT_SIZE);
    map_pointer new_map = create_map(new_map_size);
    const size_type old_buffer = m_finish.node - m_start.node + 1;
    const size_type new_buffer = old_buffer + need_buffer;

    // 另新的 map 中的指针指向原来的 buffer，并开辟新的 buffer
    map_pointer begin = new_map + ((new_map_size - new_buffer) / 2);
    map_pointer mid = begin + old_buffer;
    map_pointer end = mid + need_buffer;
    for (auto begin1 = begin, begin2 = m_start.node; begin1 != mid; ++begin1, ++begin2)
        *begin1 = *begin2;
    create_buffer(mid, end - 1);

    // 更新数据
    alloc_map::deallocate(m_map, m_size_map);
    m_map = new_map;
    m_size_map = new_map_size;
    m_start = iterator(*begin + (m_start.cur - m_start.first), begin);
    m_finish = iterator(*(mid - 1) + (m_finish.cur - m_finish.first), mid - 1);
}

template <class T>
void deque<T>::insert(iterator pos, const_reference x) {
    insert(pos, 1, x);
}

template <class T>
void deque<T>::insert(iterator pos, size_type n, const_reference x) {
    size_type elem_before = pos - m_start;
    size_type cnt = size();
    if (elem_before < cnt / 2) {
        require_capacity(n, true);
        auto new_start = m_start - n;
        auto old_start = m_start;
        pos = m_start + elem_before;
        initialize_copy(old_start, pos, new_start);
        initialize_fill_n(pos - n, n, x);
        m_start = new_start;
    }
    else {
        require_capacity(n, false);
        auto new_finish = m_finish + n;
        auto old_finish = m_finish;
        pos = m_start + elem_before;
        initialize_copy_r(pos, m_finish, pos + n);
        initialize_fill_n(pos, n, x);
        m_finish = new_finish;
    }
}

template <class T>
typename deque<T>::iterator deque<T>::erase(iterator start, iterator finish) {
    int len = size();
    int elem_before = start - m_start;
    int elem_after = m_finish - finish;
    int n = finish - start;
    if (elem_before < elem_after) {
        initialize_copy_r(m_start, start, m_start + n);
        iterator new_start = m_start + n;
        destory(m_start, new_start);
        m_start = new_start;
        return finish;
    }
    else {
        initialize_copy(finish, m_finish, start);
        iterator new_finish = m_finish - n;
        destory(new_finish, m_finish);
        m_finish = new_finish;
        return start;
    }
}



}
#endif