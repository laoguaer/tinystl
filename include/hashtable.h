#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "vector.h"
#include "pair.h"
#include "functional.h"

namespace mystl{

template <class Value>
struct hash_node {
    hash_node* next;
    Value value;
};
template <class Key, class Value, class Extract, class Equal, class HashFuc>
class hash_table;

template <class Key, class Value, class Extract, class Equal, class HashFuc>
class hash_iterator : iterator<bidirectional_iterator_tag, Key> {
public:
    typedef hash_iterator                                           iterator;
    typedef hash_table<Key, Value, Extract, Equal, HashFuc>         hashtable;
    typedef hash_node<Value>                                        hashnode;
    typedef hashnode*                                               node_ptr;
    typedef Key                                                     key_type;
    typedef Value                                                   value_type;
    typedef value_type*                                             pointer;
    typedef value_type&                                             reference;
    typedef const value_type&                                       const_reference;
    typedef typename iterator::difference_type                      difference_type;
    typedef size_t                                                  size_type;

    hash_iterator(node_ptr n, hashtable* table) : node(n), ht(table) {}
    hash_iterator(const iterator& rhs)
    {
        node = rhs.node;
        ht = rhs.ht;
    }


    bool operator==(const iterator& rhs) const { return node == rhs.node; }
    bool operator!=(const iterator& rhs) const { return node != rhs.node; }

        // 重载操作符
    reference operator*()  const { return node->value; }
    pointer   operator->() const { return &(operator*()); }

    iterator& operator++()
    {
        assert(node != nullptr);
        const node_ptr old = node;
        node = node->next;
        if (node == nullptr)
        { // 如果下一个位置为空，跳到下一个 bucket 的起始处
        auto index = ht->hash(Extract()(old->value));
        while (!node && ++index < ht->bucket_size)
            node = ht->buckets[index];
        }
        return *this;
    }
    iterator operator++(int)
    {
        iterator tmp = *this;
        ++*this;
        return tmp;
    }

private:
    node_ptr node;
    hashtable* ht;
};

#ifndef PRIME_NUM
#define PRIME_NUM 99
#endif

static constexpr size_t ht_prime_list[] = {
  101ull, 173ull, 263ull, 397ull, 599ull, 907ull, 1361ull, 2053ull, 3083ull,
  4637ull, 6959ull, 10453ull, 15683ull, 23531ull, 35311ull, 52967ull, 79451ull,
  119179ull, 178781ull, 268189ull, 402299ull, 603457ull, 905189ull, 1357787ull,
  2036687ull, 3055043ull, 4582577ull, 6873871ull, 10310819ull, 15466229ull,
  23199347ull, 34799021ull, 52198537ull, 78297827ull, 117446801ull, 176170229ull,
  264255353ull, 396383041ull, 594574583ull, 891861923ull, 1337792887ull,
  2006689337ull, 3010034021ull, 4515051137ull, 6772576709ull, 10158865069ull,
  15238297621ull, 22857446471ull, 34286169707ull, 51429254599ull, 77143881917ull,
  115715822899ull, 173573734363ull, 260360601547ull, 390540902329ull, 
  585811353559ull, 878717030339ull, 1318075545511ull, 1977113318311ull, 
  2965669977497ull, 4448504966249ull, 6672757449409ull, 10009136174239ull,
  15013704261371ull, 22520556392057ull, 33780834588157ull, 50671251882247ull,
  76006877823377ull, 114010316735089ull, 171015475102649ull, 256523212653977ull,
  384784818980971ull, 577177228471507ull, 865765842707309ull, 1298648764060979ull,
  1947973146091477ull, 2921959719137273ull, 4382939578705967ull, 6574409368058969ull,
  9861614052088471ull, 14792421078132871ull, 22188631617199337ull, 33282947425799017ull,
  49924421138698549ull, 74886631708047827ull, 112329947562071807ull, 168494921343107851ull,
  252742382014661767ull, 379113573021992729ull, 568670359532989111ull, 853005539299483657ull,
  1279508308949225477ull, 1919262463423838231ull, 2878893695135757317ull, 4318340542703636011ull,
  6477510814055453699ull, 9716266221083181299ull, 14574399331624771603ull, 18446744073709551557ull
};

// 找出最接近并大于等于 n 的那个质数
inline size_t ht_next_prime(size_t n)
{
  const size_t* first = ht_prime_list;
  const size_t* last = ht_prime_list + PRIME_NUM;
  const size_t* pos;
  for (size_t i = 0; i < PRIME_NUM; ++i) {
    if (*(first + i) > n) {
        pos = first + i;
        break;
    }
  }
  return pos == last ? *(last - 1) : *pos;
}

template <class Key, class Value, class Extract, class Equal, class HashFuc>
class hash_table{

    friend class hash_iterator<Key,Value, Extract, Equal, HashFuc>;

public:
    typedef hash_iterator<Key, Value, Extract, Equal, HashFuc>  iterator;
    typedef hash_node<Value>                                    hashnode;
    typedef hash_table<Key, Value, Extract, Equal, HashFuc>         hashtable;
    typedef hash_node<Value>*                                       node_ptr;

    typedef Key                                                     key_type;
    typedef Value                                                   value_type;
    typedef typename iterator::pointer                              pointer;
    typedef typename iterator::reference                            reference;
    typedef typename iterator::const_reference                      const_reference;
    typedef typename iterator::difference_type                      difference_type;
    typedef typename iterator::size_type                            size_type;
    typedef mystl::vector<node_ptr>                                 bucket_type;
    typedef alloc<Value>                                            data_alloc;
    typedef alloc<hashnode>                                        node_alloc;

// private:
    bucket_type buckets;
    size_type   bucket_size;
    size_type   m_size;
    HashFuc      m_hash;
    Equal      equal;
    Extract     key;
public:
//构造函数
    explicit hash_table(size_type bucket_count = 1) : m_size(0),bucket_size(1) {
        init(bucket_count);
    }

    ~hash_table() { clear(); }
//迭代器
    iterator begin() noexcept
    {
        for (size_type n = 0; n < bucket_size; ++n)
        {
        if (buckets[n])  // 找到第一个有节点的位置就返回
            return iterator(buckets[n], this);
        }
        return end();
    }
    iterator end() noexcept { return iterator(nullptr, this); }

// 容量相关操作
    bool      empty()    const noexcept { return m_size == 0; }
    size_type size()     const noexcept { return m_size; }
    size_type max_size() const noexcept { return static_cast<size_type>(-1); }
    void clear();
    void init(size_type n);
// 插入删除操作
    pair<iterator, bool> insert_unique(const value_type& val);

// 查找
    iterator find(const key_type& key);
private:
//node
    node_ptr create_node(const value_type& val);    
    void destroy_node(node_ptr ptr);
//hash
    void rehash(size_type n);
    size_type hash(const key_type& key, size_type n) const;
    size_type hash(const key_type& key) const;
};

template <class Key, class Value, class Extract, class Equal, class HashFuc>
void hash_table<Key, Value, Extract, Equal, HashFuc>::
init(size_type n)  {
    const auto bucket_nums = ht_next_prime(n);
    try
    {
        buckets.reserve(bucket_nums);
    }
    catch (...)
    {
        bucket_size = 0;
        m_size = 0;
        throw;
    }
    bucket_size = buckets.size();    
}

template <class Key, class Value, class Extract, class Equal, class HashFuc>
typename hash_table<Key, Value, Extract, Equal, HashFuc>::node_ptr 
hash_table<Key, Value, Extract, Equal, HashFuc>::create_node(const value_type& val) 
{
    node_ptr ret = node_alloc::allocate();
    construct((value_type*)&ret->value, val);
    ret->next = nullptr;
    return ret;
}

template <class Key, class Value, class Extract, class Equal, class HashFuc>
void hash_table<Key, Value, Extract, Equal, HashFuc>::destroy_node(node_ptr ptr) 
{
    destory(ptr);
    node_alloc::deallocate(ptr);
}

// 清空 hashtable
template <class Key, class Value, class Extract, class Equal, class HashFuc>
void hash_table<Key, Value, Extract, Equal, HashFuc>::
clear()
{
  if (m_size != 0)
  {
    // for (size_type i = 0; i < bucket_size; ++i) {
    //     printf("clear : %d  %p \n", i, buckets[i]);
    // }

    for (size_type i = 0; i < bucket_size; ++i)
    {
      auto cur = buckets[i];
    //   printf("i :: %d  %p\n", i, &buckets[i]);
      while (cur)
      {
        //printf("%p \n",cur->next);
        auto next = cur->next;
        destroy_node(cur);
        cur = next;
      }
      buckets[i] = nullptr;
    }
    m_size = 0;
  }
}

// 插入 
template <class Key, class Value, class Extract, class Equal, class HashFuc>
pair<typename hash_table<Key, Value, Extract, Equal, HashFuc>::iterator, bool> 
hash_table<Key, Value, Extract, Equal, HashFuc>::
insert_unique(const value_type& value) 
{
    
    // for (size_type i = 0; i < bucket_size; ++i) {
    //     printf("before insert : %d  %p \n", i, buckets[i]);
    // }
    rehash(1);

    // if (value == 102) printf("after hash : %p\n", buckets[1]);
    auto n = hash(key(value), bucket_size);
   
    auto first = buckets[n];
    for (auto cur = first; cur; cur = cur->next)
    {
        if (equal(key(cur->value), key(value)))
        return mystl::make_pair(iterator(cur, this), false);
    }
    // 让新节点成为链表的第一个节点
    auto tmp = create_node(value);  
    tmp->next = first;
    buckets[n] = tmp;
    ++m_size;
    return mystl::make_pair(iterator(tmp, this), true);
}

template <class Key, class Value, class Extract, class Equal, class HashFuc>
void hash_table<Key, Value, Extract, Equal, HashFuc>::
rehash(size_type count) 
{
    int need_size = count + m_size;
    if (need_size > bucket_size) {
        size_t bucket_count = ht_next_prime(need_size);
        bucket_type bucket(bucket_count, nullptr);
        
        if (m_size != 0)
        {
            for (size_type i = 0; i < bucket_size; ++i)
            {
                for (auto first = buckets[i]; first; first = first->next)
                {   
                    node_ptr tmp = create_node(first->value);
                    //重新hash
                    const auto n = hash(key(first->value), bucket_count);
                    // printf(" n ::: %d\n", n);
                    //插入到新的桶中
                    auto f = bucket[n];
                    bool is_inserted = false;
                    for (auto cur = f; cur; cur = cur->next)
                    {
                        if (equal(key(cur->value), key(first->value)))
                        {
                            tmp->next = cur->next;
                            cur->next = tmp;
                            is_inserted = true;
                            break;
                        }
                    }
                    
                    // 如果目标桶节点里面的hash不一或者没有节点就插入到头结点
                    if (!is_inserted)
                    {
                        tmp->next = f;
                        bucket[n] = tmp;
                    }
                    //if (bucket[i]) printf("bucket : %d  value: %d  add : %p\n", i, bucket[i]->value.second, bucket[i]);
                }
            }
            // printf("\n");
            // for (size_type i = 0; i < bucket_size; ++i) {
            //     printf("a : %d  %p \n", i, bucket[i]);
            // }
            // todo
            // printf("before %p %p \n", buckets.begin(), buckets.end());

            auto t1 = buckets._first ;
            auto t2 = buckets._finish;
            auto t3 = buckets._end_store;
            buckets._first = bucket._first;
            buckets._finish = bucket._finish;
            buckets._end_store = bucket._end_store;
            bucket._first = t1;
            bucket._finish = t2;
            bucket._end_store = t3;

            bucket_size = buckets.size();

            
            // printf("after %p %p \n", buckets.begin(), buckets.end());

            // for (size_type i = 0; i < bucket_size; ++i) {
            //     printf("after swap : %d  %p \n", i, buckets[i]);
            // }
        }
    }
}

template <class Key, class Value, class Extract, class Equal, class HashFuc>
typename hash_table<Key, Value, Extract, Equal, HashFuc>::size_type hash_table<Key, Value, Extract, Equal, HashFuc>::
hash(const key_type& val) const
{
    return m_hash(val);
}

template <class Key, class Value, class Extract, class Equal, class HashFuc>
typename hash_table<Key, Value, Extract, Equal, HashFuc>::size_type hash_table<Key, Value, Extract, Equal, HashFuc>::
hash(const key_type& val, size_type n) const
{
    return m_hash(val) % n;
}


template <class Key, class Value, class Extract, class Equal, class HashFuc>
typename hash_table<Key, Value, Extract, Equal, HashFuc>::iterator
hash_table<Key, Value, Extract, Equal, HashFuc>::
find(const key_type& tkey) {
    const auto n = hash(tkey);
    node_ptr first = buckets[n];
    for (; first && !equal(key(first->value), tkey); first = first->next) {}
    return iterator(first, this);
}


}
#endif