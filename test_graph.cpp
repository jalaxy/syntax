#include <iostream>
#include "graph.h"
using namespace std;
void print(graph<int, int> &g)
{
    for (int i = 0; i < g.size(); i++)
    {
        cout << g[i].data << " " << &g[i] << "\t";
        for (int j = 0; j < g[i].size(); j++)
            cout << g[i][j].get_to() << " " << g[i][j].data << "\t";
        cout << endl;
    }
    cout << endl;
}
int main()
{
    int a = 1, b = 2;
    graph<int, int> g;
    g.add_vertex(0);
    g.add_vertex(1);
    g.add_vertex(2);
    g.add_vertex(3);
    g.add_vertex(4);
    g.add_edge(0, 0, 2);
    g.add_edge(0, 1, 3);
    g.add_edge(0, 2, 1);
    g.add_edge(0, 3, 4);
    g.add_edge(0, 4, 5);
    g.add_edge(1, 0, 6);
    g.add_edge(4, 0, 8);
    g.add_edge(2, 1, 7);
    g.add_edge(2, 3, 9);
    g.add_edge(2, 3, 0);
    g.add_edge(3, 2, 3);
    graph<int, int> gg(g), ggg;
    ggg = gg;
    ggg = g;
    g.remove_edge(2, 3);
    g.remove_edge(0, 1);
    g.remove_edge(1, 4);
    g.remove_vertex(1);
    g.remove_vertex(0);
    print(g);
    print(ggg);
    graph<int, int> gggg = g + ggg;
    print(gggg);
    for (int i = 0; i < gggg.size(); i++)
        gggg[i].sort();
    print(gggg);
    gggg.sort();
    print(gggg);
    return 0;
}