#ifndef UTIL_H
#define UTIL_H

namespace mystl {

template<class T>
struct remove_reference {
    typedef T   type;
};

template <class T>
struct remove_reference<T&> {
    typedef T   type;
};

template <class T>
struct remove_reference<T&&> {
    typedef T   type;
};

template<class T>
typename  remove_reference<T>::type&& 
  move(T&& arg) {
    return static_cast<typename remove_reference<T>::type&&>(arg);
}


// T -> T& ?????????????
// error: cannot bind rvalue reference of type ‘int&&’ to
//        lvalue of type ‘mystl::remove_reference<int>::type’ {aka ‘int’}
template<class T>
T&& 
  forward(typename remove_reference<T>::type& arg) {
    return static_cast<T&&>(arg);
}


// T -> T
//  此处可以 不对arg进行static_cast ?
template<class T>
T&& 
  forward(typename remove_reference<T>::type&& arg) {
    return static_cast<T&&>(arg);
}

//   template<typename _Tp>
//     constexpr _Tp&&
//     forward(typename remove_reference<_Tp>::type& __t) noexcept
//     { return static_cast<_Tp&&>(__t); }


//   template<typename _Tp>
//     constexpr _Tp&&
//     forward(typename remove_reference<_Tp>::type&& __t) noexcept
//     {
//       return static_cast<_Tp&&>(__t);
//     }


}
#endif