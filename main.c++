#include "allocator.h"
#include <iostream>
#include <cstdio>
#include <vector>
#include <ctime>
#include <list>
#include <deque>
#include "list.h"
#include "initialized.h"
#include "vector.h"
#include "deque.h"
#include "stack.h"
#include "queue.h"
#include <queue>
#include <algorithm>
#include "pair.h"
#include <set>
#include "rb_tree.h"
#include "set.h"
#include "map.h"
#include <map>
#include "hashtable.h"
#include "unordered_set.h"
#include "unordered_map.h"
#include <typeinfo>
#include <memory>
#include "util.h"
using namespace mystl;

class Test{
  public:
    Test() : val_(0) {};
    explicit Test(int i) : val_(i) {}
    Test(Test& rhs) : val_(rhs.val_) { printf("copy construct!\n"); }
    Test(Test&& rhs)  noexcept : val_(rhs.val_) { printf("move construct!\n"); }

    int val_;
};

auto main() -> int {    
    printf("%d\n", __cplusplus);
}
