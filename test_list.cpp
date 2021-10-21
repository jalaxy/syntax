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
    list<int> l;
    l += 1;
    print(l);
    l.set_reduce();
    print(l);
    return 0;
}