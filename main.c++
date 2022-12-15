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
#include <list>
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
#include <algorithm>
using namespace mystl;

class test{
  public:
    test() : val(0) {};
    test(int i) : val(i) {}
    test(test& rhs) : val(rhs.val) { printf("copy construct!\n"); }
    test(test&& rhs) : val(rhs.val) { printf("move construct!\n"); }

    int val;
};

int main() {    
    printf("%d\n", __cplusplus);
}
