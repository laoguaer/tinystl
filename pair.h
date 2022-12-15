#ifndef PAIR_H
#define PAIR_H
namespace mystl {

template <class Ty1, class Ty2>
struct pair
{
    typedef Ty1    first_type;
    typedef Ty2    second_type;

    first_type first;    // 保存第一个数据
    second_type second;  // 保存第二个数据

    // default constructiable
    template <class Other1 = Ty1, class Other2 = Ty2>
        constexpr pair()
        : first(), second()
    {
    }

    // implicit constructiable for this type
    template <class U1 = Ty1, class U2 = Ty2>
        constexpr pair(const Ty1& a, const Ty2& b)
        : first(a), second(b)
    {
    }


    pair(const pair& rhs): first(rhs.first), second(rhs.second) {};


    // implicit constructiable for other pair
    template <class Other1, class Other2>
        constexpr pair(const pair<Other1, Other2>& other)
        : first(other.first),
        second(other.second)
    {
    }



    // copy assign for this pair
    pair& operator=(const pair& rhs)
    {
        if (this != &rhs)
        {
        first = rhs.first;
        second = rhs.second;
        }
        return *this;
    }


    // copy assign for other pair
    template <class Other1, class Other2>
    pair& operator=(const pair<Other1, Other2>& other)
    {
        first = other.first;
        second = other.second;
        return *this;
    }



    ~pair() = default;


};

// 全局函数，让两个数据成为一个 pair
template <class Ty1, class Ty2>
pair<Ty1, Ty2> make_pair(const Ty1& first, const Ty2& second)
{
  return pair<Ty1, Ty2>(first, second);
}


}
#endif

