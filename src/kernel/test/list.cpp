#include <assert.h>
#include <stdio.h>
#include "std/list.h"

bool test_add_remove() {
    kernel::list<int> mylist;
    assert(mylist.empty());
    assert(mylist.size() == 0);
    mylist.push_front(1);
    assert(!mylist.empty());
    assert(mylist.size() == 1);
    assert(mylist.front() == 1);
    assert(mylist.back() == 1);
    mylist.push_front(2);
    assert(!mylist.empty());
    assert(mylist.size() == 2);
    assert(mylist.front() == 2);
    assert(mylist.back() == 1);
    mylist.clear();
    assert(mylist.empty());
    assert(mylist.size() == 0);

    mylist.push_front(1);
    mylist.push_front(2);
    mylist.push_front(3);
    auto b = mylist.begin().next();
    auto c = mylist.insert(b, 4);
    assert(mylist.size() == 4);
    assert(mylist[0] == 3);
    assert(mylist[1] == 4);
    assert(mylist[2] == 2);
    assert(mylist[3] == 1);
    mylist.erase(c);
    assert(mylist.size() == 3);
    assert(mylist[0] == 3);
    assert(mylist[1] == 2);
    assert(mylist[2] == 1);

    mylist.clear();
    assert(mylist.empty());
    assert(mylist.size() == 0);
    return true;
}

int main(int argc, char* argv[]) {
    if (!test_add_remove()) return 1;
    return 0;
}