#include <iostream>
#include "list.h"
using namespace std;
void print(list<int> l)
{
    for (int i = 0; i < l.size(); i++)
        cout << l[i] << " ";
    cout << " top " << l.top() << " ";
    cout << "undertop " << l.undertop() << " ";
    cout << "size " << l.size() << " ";
    cout << "empty? " << l.empty() << endl;
}
int main()
{
    list<int> l, m;
    l += 1;
    l += 1;
    l += 3;
    l += 2;
    m += 1;
    m += 2;
    m += 3;
    m += 1;
    cout << (l < m) << endl;
    return 0;
}