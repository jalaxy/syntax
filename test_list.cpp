#include <iostream>
#include "list.h"
using namespace std;
void print(list<int> l)
{
    for (int i = 0; i < l.size(); i++)
        cout << l[i] << " ";
    cout << " front " << l.front() << " ";
    cout << "afterfront " << l.afterfront() << " ";
    cout << "size " << l.size() << " ";
    cout << "empty? " << l.empty() << endl;
}
int main()
{
    list<int> l, m;
    print(l);
    l += 1;
    print(l);
    l += 2;
    print(l);
    l += 3;
    l += 4;
    print(l);
    l += 5;
    l += 6;
    l += 7;
    l += 8;
    print(l);
    l += 2;
    print(l);
    l += 3;
    print(l);
    l.pop_back();
    print(l);
    l.pop_front();
    print(l);
    l.pop_front();
    print(l);
    l.remove(1);
    print(l);
    return 0;
}