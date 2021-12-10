/****************************************************************
 * @file graph.h
 * @author Jiang, Xingyu (chinajxy@outlook.com)
 * @brief Implemention of a graph class, using adjancency list
 * @version 0.1
 * @date 2021-10-15
 * 
 * @copyright Copyright (c) 2021
 * 
 ****************************************************************/

#pragma once

#include "list.h"

template <class Tv, class Te>
class vertex;
template <class Tv, class Te>
class edge;

template <class Tv, class Te>
class graph
{
private:
    list<vertex<Tv, Te>> vertices;
    void copy(const graph &b)
    {
        vertices = b.vertices;
        ((graph &)b).mapping(*this, 0);
        for (int i = 0; i < vertices.size(); i++)
            for (int j = 0; j < vertices[i].edges.size(); j++)
                vertices[i].edges[j].to = ((graph &)b).vertices[i].edges[j].to->aux;
    }

public:
    graph() { vertices = list<vertex<Tv, Te>>(); }
    graph(const graph &b) { copy(b); }
    const graph &operator=(const graph &b)
    {
        if (&b != this)
            copy(b);
        return *this;
    }
    graph operator+(const graph &b) const
    {
        graph c(*this);
        int base = vertices.size();
        for (int i = 0; i < b.vertices.size(); i++)
            c.vertices.append(((graph &)b)[i]);
        b.mapping(c, base);
        for (int i = 0; i < b.vertices.size(); i++)
            for (int j = 0; j < ((graph &)b).vertices[i].edges.size(); j++)
                c.vertices[base + i].edges[j].to = ((graph &)b).vertices[i].edges[j].to->aux;
        return c;
    }
    vertex<Tv, Te> &operator[](int i) { return vertices[i]; }
    int size() const { return vertices.size(); }
    vertex<Tv, Te> &top() const { return vertices.top(); }
    void sort() { vertices.sort(0, vertices.size() - 1); }
    void merge(graph &b) { vertices.merge(&b.vertices); }
    void mapping(const graph<Tv, Te> &b, int base) const
    {
        for (int i = 0; i < size() && base + i < ((graph &)b).vertices.size(); i++)
            ((graph &)*this).vertices[i].aux = &((graph &)b).vertices[base + i];
    }
    void add_vertex(const Tv &data) { vertices.append(vertex<Tv, Te>(data)); }
    void add_edge(int i, int j, const Te &data)
    {
        vertices[i].edges.append(edge<Tv, Te>(data));
        vertices[i].edges[vertices[i].edges.size() - 1].to = &vertices[j];
    }
    void add_edge(vertex<Tv, Te> &p, vertex<Tv, Te> &q, const Te &data)
    {
        p.edges.append(edge<Tv, Te>(data));
        p.edges[p.edges.size() - 1].to = &q;
    }
    void remove_vertex(int i)
    {
        for (int j = 0; j < vertices.size(); j++)
            remove_edge(j, i);
        if (i < vertices.size())
            vertices.remove(i);
    }
    void remove_edge(int i, int j)
    {
        if (i < vertices.size())
            for (int k = 0; k < vertices[i].edges.size(); k++)
                if (vertices[i].edges[k].to == &vertices[j])
                    vertices[i].edges.remove(k--);
    }
};

template <class Tv, class Te>
class vertex
{
private:
    list<edge<Tv, Te>> edges;
    friend class graph<Tv, Te>;

public:
    Tv data;
    vertex *aux; // auxiliary space for copying, storing latest related vertex address
                 // or some other temporary auxiliary use
    vertex(const Tv &vdata)
    {
        data = Tv(vdata);
        edges = list<edge<Tv, Te>>();
        aux = NULL;
    }
    int size() const { return edges.size(); }
    void sort() { edges.sort(0, edges.size() - 1); }
    void merge(vertex &b) { edges.merge(&b.edges); }
    void set_reduce() { edges.set_reduce(); }
    edge<Tv, Te> &operator[](int i) { return edges[i]; }
    bool operator<(const vertex &b) const { return data < b.data; }
};

template <class Tv, class Te>
class edge
{
private:
    friend class graph<Tv, Te>;

public:
    vertex<Tv, Te> *to;
    Te data;
    edge(const Te &vdata)
    {
        data = Te(vdata);
        to = NULL;
    }
    bool operator<(const edge &b) const
    {
        if (data == b.data)
            return to < b.to;
        return data < b.data;
    }
    bool operator==(const edge &b) const { return to == b.to && data == b.data; }
};
