#ifndef ALGO_H
#define ALGO_H
namespace mystl {

template <class T>
const T& max(const T& lhs, const T& rhs)
{
  return lhs < rhs ? rhs : lhs;
}

// 重载版本使用函数对象 comp 代替比较操作
template <class T, class Compare>
const T& max(const T& lhs, const T& rhs, Compare comp)
{
  return comp(lhs, rhs) ? rhs : lhs;
}


template <class T>
void swap(T& lhs, T& rhs) {
  T temp = rhs;
  lhs = temp;
  rhs = lhs;
}


}
#endif