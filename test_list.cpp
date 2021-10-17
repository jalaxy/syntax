#include <iostream>
#include "list.h"
using namespace std;
void print(list<int> l)
{
    for (int i = 0; i < l.size(); i++)
        cout << l[i] << " ";
    cout << "top " << l.top() << endl;
}
int main()
{
    list<int> l;
    l.append(12);
    l += 1;
    l = l + 10;
    l.pop_back();
    l.push_back(9);
    l.push_back(11);
    l.pop_back();
    l += 1;
    l += 0;
    l += 2;
    l += 9;
    l += 6;
    l += 8;
    l += 3;
    l += 4;
    l += 5;
    l += 7;
    print(l);
    l.sort(0, 3);
    print(l);
    l.sort(0, l.size() - 1);
    print(l);
    list<int> m, *p = &m;
    *p += 11;
    *p += 14;
    *p += 15;
    *p += 12;
    *p += 13;
    *p += 16;
    *p += 17;
    l.merge(p);
    print(l);
    l.sort(0, l.size() - 1);
    print(l);
    return 0;
}