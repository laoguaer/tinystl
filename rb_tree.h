#ifndef RB_TREE_H
#define RB_TREE_H

#include "allocator.h"
#include "initialized.h"
#include "iterator.h"
#include "algo.h"
#include "pair.h"
#include "functional.h" 

namespace mystl {

// rb tree 节点颜色的类型

typedef bool rb_tree_color_type;

static constexpr rb_tree_color_type rb_tree_red   = false;
static constexpr rb_tree_color_type rb_tree_black = true;


// forward declaration
template <class T> struct rb_tree_node;
template <class T> struct rb_tree_iterator;

template <class T> 
struct rb_tree_iterator : public iterator<bidirectional_iterator_tag, T> {
    typedef rb_tree_iterator                            self;
    typedef self                                        iterator;
    typedef rb_tree_node<T>*                            link_type;

    typedef typename self::iterator_category            iterator_category;
    typedef typename self::value_type                   value_type;
    typedef typename self::pointer                      pointer;
    typedef typename self::reference                    reference;
    typedef typename self::const_reference              const_reference;
    typedef typename self::difference_type              difference_type;
    typedef typename self::size_type                    size_type;

      // 构造函数
    rb_tree_iterator() {}
    rb_tree_iterator(link_type x) : node(x) {}
    rb_tree_iterator(const iterator& rhs) { node = rhs.node; }
    

      // 重载操作符
    reference operator*()  const { return node->value; }
    pointer   operator->() const { return &(operator*()); }

    self& operator++()
    {
        if (node->right != nullptr)
        { 
            node = rb_tree_min(node->right);
        }
        else
        {  // 如果没有右子节点
            auto y = node->parent;
            while (y->right == node)
            {
                node = y;
                y = y->parent;
            }
            if (node->right != y)  // 应对“寻找根节点的下一节点，而根节点没有右子节点”的特殊情况
                node = y;
        }
        return *this;
    }
    self operator++(int)
    {
        self tmp(*this);
        ++*this;
        return tmp;
    }

    self& operator--()
    {
        if (node->parent->parent == node && rb_tree_is_red(node))
        { // 如果 node 为 header
            node = node->right;  // 指向整棵树的 max 节点
        }
        else if (node->left != nullptr)
        {
            node = rb_tree_max(node->left);
        }
        else
        {  // 非 header 节点，也无左子节点
            auto y = node->parent;
            while (node == y->left)
            {
                node = y;
                y = y->parent;
            }
            node = y;
        }
        return *this;
    }
    self operator--(int)
    {
        self tmp(*this);
        --*this;
        return tmp;
    }

    bool operator==(const self& rhs) { return node == rhs.node; }
    bool operator!=(const self& rhs) { return !(node == rhs.node); }

    link_type node;
};


template <class T>
struct rb_tree_node {
    typedef rb_tree_node<T>*    link_type;
    typedef rb_tree_color_type    color_type;
    
    
    link_type left;
    link_type right;
    link_type parent;
    color_type  color;
    T value;
};

// tree algorithm

template <class NodePtr>
NodePtr rb_tree_min(NodePtr x) noexcept
{
  while (x->left != nullptr)
    x = x->left;
  return x;
}

template <class NodePtr>
NodePtr rb_tree_max(NodePtr x) noexcept
{
  while (x->right != nullptr)
    x = x->right;
  return x;
}

template <class NodePtr>
bool rb_tree_is_lchild(NodePtr node) noexcept
{
  return node == node->parent->left;
}

template <class NodePtr>
bool rb_tree_is_red(NodePtr node) noexcept
{
  return node->color == rb_tree_red;
}

template <class NodePtr>
void rb_tree_set_black(NodePtr& node) noexcept
{
  node->color = rb_tree_black;
}

template <class NodePtr>
void rb_tree_set_red(NodePtr& node) noexcept
{
  node->color = rb_tree_red;
}

template <class NodePtr>
NodePtr rb_tree_next(NodePtr node) noexcept
{
  if (node->right != nullptr)
    return rb_tree_min(node->right);
  while (!rb_tree_is_lchild(node))
    node = node->parent;
  return node->parent;
}


template <class NodePtr>
void rb_tree_insert_rebalance(NodePtr x, NodePtr& root) noexcept
{
  rb_tree_set_red(x);  // 新增节点为红色
  while (x != root && rb_tree_is_red(x->parent))
  {
    if (rb_tree_is_lchild(x->parent))
    { // 如果父节点是左子节点
      auto uncle = x->parent->parent->right;
      if (uncle != nullptr && rb_tree_is_red(uncle))
      { // case 3: 父节点和叔叔节点都为红
        rb_tree_set_black(x->parent);
        rb_tree_set_black(uncle);
        x = x->parent->parent;
        rb_tree_set_red(x);
      }
      else
      { // 无叔叔节点或叔叔节点为黑
        if (!rb_tree_is_lchild(x))
        { // case 4: 当前节点 x 为右子节点
          x = x->parent;
          rb_tree_rotate_left(x, root);
        }
        // 都转换成 case 5： 当前节点为左子节点
        rb_tree_set_black(x->parent);
        rb_tree_set_red(x->parent->parent);
        rb_tree_rotate_right(x->parent->parent, root);
        break;
      }
    }
    else  // 如果父节点是右子节点，对称处理
    { 
      auto uncle = x->parent->parent->left;
      if (uncle != nullptr && rb_tree_is_red(uncle))
      { // case 3: 父节点和叔叔节点都为红
        rb_tree_set_black(x->parent);
        rb_tree_set_black(uncle);
        x = x->parent->parent;
        rb_tree_set_red(x);
        // 此时祖父节点为红，可能会破坏红黑树的性质，令当前节点为祖父节点，继续处理
      }
      else
      { // 无叔叔节点或叔叔节点为黑
        if (rb_tree_is_lchild(x))
        { // case 4: 当前节点 x 为左子节点
          x = x->parent;
          rb_tree_rotate_right(x, root);
        }
        // 都转换成 case 5： 当前节点为左子节点
        rb_tree_set_black(x->parent);
        rb_tree_set_red(x->parent->parent);
        rb_tree_rotate_left(x->parent->parent, root);
        break;
      }
    }
  }
  rb_tree_set_black(root);  // 根节点永远为黑
}

/*----------------------------------------*\
|     p                         p          |
|    / \                       / \         |
|   d   x      rotate right   d   y        |
|      / \     ===========>      / \       |
|     y   a                     b   x      |
|    / \                           / \     |
|   b   c                         c   a    |
\*----------------------------------------*/
// 右旋，参数一为右旋点，参数二为根节点
template <class NodePtr>
void rb_tree_rotate_right(NodePtr x, NodePtr& root) noexcept
{
  auto y = x->left;
  x->left = y->right;
  if (y->right)
    y->right->parent = x;
  y->parent = x->parent;

  if (x == root)
  { // 如果 x 为根节点，让 y 顶替 x 成为根节点
    root = y;
  }
  else if (rb_tree_is_lchild(x))
  { // 如果 x 是右子节点
    x->parent->left = y;
  }
  else
  { // 如果 x 是左子节点
    x->parent->right = y;
  }
  // 调整 x 与 y 的关系
  y->right = x;                      
  x->parent = y;
}


/*---------------------------------------*\
|       p                         p       |
|      / \                       / \      |
|     x   d    rotate left      y   d     |
|    / \       ===========>    / \        |
|   a   y                     x   c       |
|      / \                   / \          |
|     b   c                 a   b         |
\*---------------------------------------*/
// 左旋，参数一为左旋点，参数二为根节点
template <class NodePtr>
void rb_tree_rotate_left(NodePtr x, NodePtr& root) noexcept
{
  auto y = x->right;  // y 为 x 的右子节点
  x->right = y->left;
  if (y->left != nullptr)
    y->left->parent = x;
  y->parent = x->parent;

  if (x == root)
  { // 如果 x 为根节点，让 y 顶替 x 成为根节点
    root = y;
  }
  else if (rb_tree_is_lchild(x))
  { // 如果 x 是左子节点
    x->parent->left = y;
  }
  else
  { // 如果 x 是右子节点
    x->parent->right = y;
  }
  // 调整 x 与 y 的关系
  y->left = x;  
  x->parent = y;
}


template <class NodePtr>
NodePtr rb_tree_erase_rebalance(NodePtr z, NodePtr& root, NodePtr& leftmost, NodePtr& rightmost)
{
  // y 是可能的替换节点，指向最终要删除的节点
  auto y = (z->left == nullptr || z->right == nullptr) ? z : rb_tree_next(z);
  // x 是 y 的一个独子节点或 NIL 节点
  auto x = y->left != nullptr ? y->left : y->right;
  // xp 为 x 的父节点
  NodePtr xp = nullptr;

  // y != z 说明 z 有两个非空子节点，此时 y 指向 z 右子树的最左节点，x 指向 y 的右子节点。
  // 用 y 顶替 z 的位置，用 x 顶替 y 的位置，最后用 y 指向 z
  if (y != z)
  {
    z->left->parent = y;
    y->left = z->left;

    // 如果 y 不是 z 的右子节点，那么 z 的右子节点一定有左孩子
    if (y != z->right)
    { // x 替换 y 的位置
      xp = y->parent;
      if (x != nullptr)
        x->parent = y->parent;

      y->parent->left = x;
      y->right = z->right;
      z->right->parent = y;
    }
    else
    {
      xp = y;
    }

    // 连接 y 与 z 的父节点 
    if (root == z)
      root = y;
    else if (rb_tree_is_lchild(z))
      z->parent->left = y;
    else
      z->parent->right = y;
    y->parent = z->parent;
    // mystl::swap(y->color, z->color);

    rb_tree_color_type t = y->color;
    y->color = z->color;
    z->color = t;   

    y = z;
  }
  // y == z 说明 z 至多只有一个孩子
  else
  { 
    xp = y->parent;
    if (x)  
      x->parent = y->parent;

    // 连接 x 与 z 的父节点
    if (root == z)
      root = x;
    else if (rb_tree_is_lchild(z))
      z->parent->left = x;
    else
      z->parent->right = x;

    // 此时 z 有可能是最左节点或最右节点，更新数据
    if (leftmost == z)
      leftmost = x == nullptr ? xp : rb_tree_min(x);
    if (rightmost == z)
      rightmost = x == nullptr ? xp : rb_tree_max(x);
  }

  // 此时，y 指向要删除的节点，x 为替代节点，从 x 节点开始调整。
  // 如果删除的节点为红色，树的性质没有被破坏，否则按照以下情况调整（x 为左子节点为例）：
  // case 1: 兄弟节点为红色，令父节点为红，兄弟节点为黑，进行左（右）旋，继续处理
  // case 2: 兄弟节点为黑色，且两个子节点都为黑色或 NIL，令兄弟节点为红，父节点成为当前节点，继续处理
  // case 3: 兄弟节点为黑色，左子节点为红色或 NIL，右子节点为黑色或 NIL，
  //         令兄弟节点为红，兄弟节点的左子节点为黑，以兄弟节点为支点右（左）旋，继续处理
  // case 4: 兄弟节点为黑色，右子节点为红色，令兄弟节点为父节点的颜色，父节点为黑色，兄弟节点的右子节点
  //         为黑色，以父节点为支点左（右）旋，树的性质调整完成，算法结束
  if (!rb_tree_is_red(y))
  { // x 为黑色时，调整，否则直接将 x 变为黑色即可
    while (x != root && (x == nullptr || !rb_tree_is_red(x)))
    {
      if (x == xp->left)
      { // 如果 x 为左子节点
        auto brother = xp->right;
        if (rb_tree_is_red(brother))
        { // case 1
          rb_tree_set_black(brother);
          rb_tree_set_red(xp);
          rb_tree_rotate_left(xp, root);
          brother = xp->right;
        }
        // case 1 转为为了 case 2、3、4 中的一种
        if ((brother->left == nullptr || !rb_tree_is_red(brother->left)) &&
            (brother->right == nullptr || !rb_tree_is_red(brother->right)))
        { // case 2
          rb_tree_set_red(brother);
          x = xp;
          xp = xp->parent;
        }
        else
        { 
          if (brother->right == nullptr || !rb_tree_is_red(brother->right))
          { // case 3
            if (brother->left != nullptr)
              rb_tree_set_black(brother->left);
            rb_tree_set_red(brother);
            rb_tree_rotate_right(brother, root);
            brother = xp->right;
          }
          // 转为 case 4
          brother->color = xp->color;
          rb_tree_set_black(xp);
          if (brother->right != nullptr)  
            rb_tree_set_black(brother->right);
          rb_tree_rotate_left(xp, root);
          break;
        }
      }
      else  // x 为右子节点，对称处理
      { 
        auto brother = xp->left;
        if (rb_tree_is_red(brother))
        { // case 1
          rb_tree_set_black(brother);
          rb_tree_set_red(xp);
          rb_tree_rotate_right(xp, root);
          brother = xp->left;
        }
        if ((brother->left == nullptr || !rb_tree_is_red(brother->left)) &&
            (brother->right == nullptr || !rb_tree_is_red(brother->right)))
        { // case 2
          rb_tree_set_red(brother);
          x = xp;
          xp = xp->parent;
        }
        else
        {
          if (brother->left == nullptr || !rb_tree_is_red(brother->left))
          { // case 3
            if (brother->right != nullptr)
              rb_tree_set_black(brother->right);
            rb_tree_set_red(brother);
            rb_tree_rotate_left(brother, root);
            brother = xp->left;
          }
          // 转为 case 4
          brother->color = xp->color;
          rb_tree_set_black(xp);
          if (brother->left != nullptr)  
            rb_tree_set_black(brother->left);
          rb_tree_rotate_right(xp, root);
          break;
        }
      }
    }
    if (x != nullptr)
      rb_tree_set_black(x);
  }
  return y;
}




template <class Key, class Value, class KeyOfValue, class Compare> 
class rb_tree {
public:
  // rb_tree 的嵌套型别定义 
    typedef rb_tree_iterator<Value>                 iterator;
    typedef typename iterator::link_type            link_type;

    typedef mystl::alloc<rb_tree_node<Value> >      node_allocator;
    typedef mystl::alloc<Value>                     data_allocator;

    typedef typename iterator::pointer              pointer;
    typedef typename iterator::reference            reference;
    typedef typename iterator::const_reference      const_reference;
    typedef typename iterator::size_type            size_type;
    typedef typename iterator::difference_type      difference_type;

    typedef Key                                     key_type;
    typedef Value                                   value_type;
    typedef Compare                                 key_compare;


private:    
    key_compare comp;
    size_type m_cnt;
    link_type m_header;

private:
  // 以下三个函数用于取得根节点，最小节点和最大节点
    link_type& root()      const { return m_header->parent; }
    link_type& leftmost()  const { return m_header->left; }
    link_type& rightmost() const { return m_header->right; }
    key_type key(const value_type& val) const { return KeyOfValue()(val); }  

public:
// 构造函数 和 内存分配函数
    rb_tree() {
        m_header = node_allocator::allocate(1);
        m_header->color = rb_tree_red;  // m_header 节点颜色为红，与 root 区分
        root() = nullptr;
        leftmost() = m_header;
        rightmost() = m_header;
        m_cnt = 0;
    }
    ~rb_tree() { clear(); }

    void clear();
    link_type create_node(value_type x);
    void destroy_node(link_type x);

    iterator begin() noexcept 
    { return leftmost(); }

    iterator end() noexcept
    { return m_header; }


//  元素操作函数

public: 
    pair<iterator, bool>insert_unique(const value_type& x);

    iterator  erase(iterator hint);
private:
    void erase_since(link_type x);

    mystl::pair<mystl::pair<link_type, bool>, bool> 
        get_insert_unique_pos(const key_type& key);

    // insert value / insert node
    iterator insert_value_at(link_type x, const value_type& value, bool add_to_left);

// 查找
public:
  iterator find(const key_type& key);

//遍历
  link_type get_header() const { return m_header; }
  void for_each(link_type node) {
    if (node == nullptr) return;
    for_each(node->left);
    printf("%d ", key(node->value));
    for_each(node->right);
  }

// 容量函数
  size_type size() const { return m_cnt; }

};

// 内存分配析构函数
template <class Key, class Value, class KeyOfValue, class Compare> 
typename rb_tree<Key, Value, KeyOfValue, Compare>::link_type
rb_tree<Key, Value, KeyOfValue, Compare>::create_node(value_type x) 
{
    link_type node = node_allocator::allocate();
    construct(&node->value, x);
    return node;
}

template <class Key, class Value, class KeyOfValue, class Compare> 
void 
rb_tree<Key, Value, KeyOfValue, Compare>::destroy_node(link_type x) {
    destory(&x->value);
    node_allocator::deallocate(x);
}

// 插入删除函数 ---------------------------------------------------
template <class Key, class Value, class KeyOfValue, class Compare> 
void rb_tree<Key, Value, KeyOfValue, Compare>::clear() {
    if (m_cnt != 0)
    {
        erase_since(root());
        leftmost() = m_header;
        root() = nullptr;
        rightmost() = m_header;
        m_cnt = 0;
    }
}

// 插入新值，节点键值不允许重复，返回一个 pair，若插入成功，pair 的第二参数为 true，否则为 false
template <class Key, class Value, class KeyOfValue, class Compare> 
void rb_tree<Key, Value, KeyOfValue, Compare>::erase_since(link_type x) {
    while (x != nullptr)
    {
        erase_since(x->right);
        auto y = x->left;
        destroy_node(x);
        x = y;
    }
}


// 删除 hint 位置的节点
template <class Key, class Value, class KeyOfValue, class Compare>
typename rb_tree<Key, Value, KeyOfValue, Compare>::iterator
rb_tree<Key, Value, KeyOfValue, Compare>::
erase(iterator hint)
{
  auto node = hint.node;
  iterator next(node);
  ++next;
  
  rb_tree_erase_rebalance(hint.node, root(), leftmost(), rightmost());
  destroy_node(node);
  --m_cnt;
  return next;
}


template <class Key, class Value, class KeyOfValue, class Compare>
mystl::pair<typename rb_tree<Key, Value, KeyOfValue, Compare>::iterator, bool>
rb_tree<Key, Value, KeyOfValue, Compare>::
insert_unique(const value_type& value)
{
  //THROW_LENGTH_ERROR_IF(m_cnt > max_size() - 1, "rb_tree<T, Comp>'s size too big");
  auto res = get_insert_unique_pos(key(value));
  if (res.second)
  { // 插入成功
    return mystl::make_pair(insert_value_at(res.first.first, value, res.first.second), true);
  }
  return mystl::make_pair(res.first.first, false);
}

// get_insert_unique_pos 函数
template <class Key, class Value, class KeyOfValue, class Compare> 
mystl::pair<mystl::pair<typename rb_tree<Key, Value, KeyOfValue, Compare>::link_type, bool>, bool>
rb_tree<Key, Value, KeyOfValue, Compare>::get_insert_unique_pos(const key_type& tkey)
{ // 返回一个 pair，第一个值为一个 pair，包含插入点的父节点和一个 bool 表示是否在左边插入，
  // 第二个值为一个 bool，表示是否插入成功
  auto x = root();
  auto y = m_header;
  bool add_to_left = true;  // 树为空时也在 header_ 左边插入
  while (x != nullptr)
  {
    y = x;
    add_to_left = comp(tkey,key(x->value));
    x = add_to_left ? x->left : x->right;
  }
  iterator j = iterator(y);  // 此时 y 为插入点的父节点
  if (add_to_left)
  {
    if (y == m_header || j == begin())
    { // 如果树为空树或插入点在最左节点处，肯定可以插入新的节点
      return mystl::make_pair(mystl::make_pair(y, true), true);
    }
    else
    { // 否则，如果存在重复节点，那么 --j 就是重复的值
      --j;
    }
  }
  if (comp(key(*j), tkey))  
  { // 表明新节点没有重复
    return mystl::make_pair(mystl::make_pair(y, add_to_left), true);
  }
  // 进行至此，表示新节点与现有节点键值重复
  return mystl::make_pair(mystl::make_pair(y, add_to_left), false);
}

// insert_value_at 函数
// x 为插入点的父节点， value 为要插入的值，add_to_left 表示是否在左边插入
template <class Key, class Value, class KeyOfValue, class Compare> 
typename rb_tree<Key, Value, KeyOfValue, Compare>::iterator
rb_tree<Key, Value, KeyOfValue, Compare>::
insert_value_at(link_type x, const value_type& value, bool add_to_left)
{
  link_type node = create_node(value);
  node->parent = x;
  auto base_node = node;
  if (x == m_header)
  {
    root() = base_node;
    leftmost() = base_node;
    rightmost() = base_node;
  }
  else if (add_to_left)
  {
    x->left = base_node;
    if (leftmost() == x)
      leftmost() = base_node;
  }
  else
  {
    x->right = base_node;
    if (rightmost() == x)
      rightmost() = base_node;
  }
  rb_tree_insert_rebalance(base_node, root());
  ++m_cnt;
  return iterator(node);
}

// 查找键值为 k 的节点，返回指向它的迭代器
template <class Key, class Value, class KeyOfValue, class Compare>
typename rb_tree<Key, Value, KeyOfValue, Compare>::iterator
rb_tree<Key, Value, KeyOfValue, Compare>::
find(const key_type& tkey)
{
  auto y = m_header;  // 最后一个不小于 key 的节点
  auto x = root();
  while (x != nullptr)
  {
    if (!comp(key(x->value), tkey))
    { // key 小于等于 x 键值，向左走
      y = x, x = x->left;
    }
    else
    { // key 大于 x 键值，向右走
      x = x->right;
    }
  }
  iterator j = iterator(y);
  return (j == end() || comp(tkey, key(*j))) ? end() : j;
}


}

#endif 