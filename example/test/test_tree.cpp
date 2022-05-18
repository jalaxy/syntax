#include <iostream>
#include "list.h"
using namespace std;
struct tree
{
    int x;
    list<tree> subtree;
};
void print(tree &tr)
{
    cout << tr.x << " ";
    for (int i = 0; i < tr.subtree.size(); i++)
        print(tr.subtree[i]);
}
int main()
{
    tree tr;
    tr.x = 0;
    tr.subtree.append({1, list<tree>()});
    tr.subtree.append({2, list<tree>()});
    tr.subtree.append({3, list<tree>()});
    tr.subtree[0].subtree.append({4, list<tree>()});
    tr.subtree[0].subtree.append({5, list<tree>()});
    print(tr);
    return 0;
}