/****************************************************************
 * @file list.h
 * @author Name (username@domain.com)
 * @brief Template of a list class, constructed by link list
 * @version 0.1
 * @date 2021-09-30
 * 
 * @copyright Copyright (c) 2021
 * 
 ****************************************************************/

#pragma once
#include <malloc.h>
#include <new>

template <class T>
class llist
{
public:
    T *p_data;
    llist *next, *pre;
};

// template class of a linked list
template <class T>
class list
{
private:
    llist<T> head, *p_rear;
    llist<T> **array;
    bool out_of_date;

    /**
     * @brief Create an object
     * 
     */
    void create()
    {
        head.p_data = (T *)malloc(sizeof(int));
        if (head.p_data == NULL)
        {
#ifdef HANDLE_MEMORY_EXCEPTION
            HANDLE_MEMORY_EXCEPTION;
#endif
        }
        *(int *)head.p_data = 0;
        head.next = head.pre = NULL;
        p_rear = &head;
        array = NULL;
        out_of_date = true;
    }

    /**
     * @brief Copy from another object
     * 
     * @param b the other object
     */
    void copy(const list<T> &b)
    {
        create();
        llist<T> *p = b.head.next;
        while (p != NULL)
        {
            append(*p->p_data);
            p = p->next;
        }
    }

    /**
     * @brief Destroy an object
     * 
     */
    void destroy()
    {
        free((int *)head.p_data);
        llist<T> *p = head.next;
        while (p != NULL)
        {
            llist<T> *q = p;
            p = p->next;
            delete q->p_data;
            free(q);
        }
        if (array != NULL)
            free(array);
    }

    /**
     * @brief Remove one node
     * 
     * @param p the pointer to the node to delete
     */
    void remove(llist<T> *p)
    {
        p->pre->next = p->next;
        if (p->next != NULL)
            p->next->pre = p->pre;
        else
            p_rear = p->pre;
        delete p->p_data;
        free(p);
        (*(int *)head.p_data)--;
        out_of_date = true;
    }

    /**
     * @brief Insert one node after pos
     * 
     * @param pos the position before the node to insert
     * @param x the value of inserted node
     */
    void insert(llist<T> *pos, const T &x)
    {
        llist<T> *p = (llist<T> *)malloc(sizeof(llist<T>));
        if (p == NULL)
        {
#ifdef HANDLE_MEMORY_EXCEPTION
            HANDLE_MEMORY_EXCEPTION;
#endif
        }
        p->p_data = new (std::nothrow) T(x);
        if (p->p_data == NULL)
        {
#ifdef HANDLE_MEMORY_EXCEPTION
            HANDLE_MEMORY_EXCEPTION;
#endif
        }
        p->pre = pos;
        p->next = pos->next;
        pos->next = p;
        if (p->next != NULL)
            p->next->pre = p;
        if (pos == p_rear)
            p_rear = p;
        (*(int *)head.p_data)++;
        out_of_date = true;
    }

    /**
     * @brief Update the mapping array
     * 
     */
    void update()
    {
        if (array != NULL)
            free(array);
        array = (llist<T> **)malloc(sizeof(llist<T> *) * *(int *)head.p_data);
        llist<T> *p = head.next;
        for (int i = 0; i < *(int *)head.p_data; i++, p = p->next)
            array[i] = p;
        out_of_date = false;
    }

public:
    /**
     * @brief Construct a new list object
     * 
     */
    list() { create(); }

    /**
     * @brief Construct a new list object
     * 
     * @param b 
     */
    list(const list<T> &b) { copy(b); }

    /**
     * @brief Destroy the list object
     * 
     */
    ~list() { destroy(); }

    /**
     * @brief Get size of the list
     * 
     * @return the size of the list
     */
    int size() const { return *(int *)head.p_data; }

    /**
     * @brief Whether this list is empty
     * 
     * @return empty or not
     */
    bool empty() const { return *(int *)head.p_data == 0; }

    /**
     * @brief Remove the idx-th element
     * 
     * @param idx the index
     */
    void remove(int idx)
    {
        if (out_of_date)
            update();
        remove(array[idx]);
    }

    /**
     * @brief Insert an element
     * 
     * @param idx the position to insert after
     * @param x the element
     */
    void insert(int idx, const T &x)
    {
        if (out_of_date)
            update();
        insert(idx < 0 ? &head : array[idx], x);
    }

    /**
     * @brief Append an element
     * 
     * @param x the element to append
     */
    void append(const T &x) { insert(p_rear, x); }

    /**
     * @brief Stack operation push
     * 
     * @param x the element to push
     */
    void push_back(const T &x) { insert(p_rear, x); }

    /**
     * @brief Push a node to front
     * 
     * @param x the element to push
     */
    void push_front(const T &x) { insert(&head, x); }

    /**
     * @brief Stack operation pop
     * 
     */
    void pop_back()
    {
        if (p_rear != &head)
            remove(p_rear);
    }

    /**
     * @brief Pop the first node
     * 
     */
    void pop_front()
    {
        if (head.next != NULL)
            remove(head.next);
    }

    /**
     * @brief Stack operation fetch-top
     * 
     * @return value of the stack top
     */
    T &top() const { return *p_rear->p_data; }

    /**
     * @brief The stack element under top
     * 
     * @return the second last element
     */
    T &undertop() const { return p_rear->pre == NULL ? *head.p_data : *p_rear->pre->p_data; }

    T &bottom() const { return head.next == NULL ? *head.p_data : *head.next->p_data; }

    /**
     * @brief 
     * 
     */
    void clear()
    {
        while (*(int *)head.p_data > 0)
            remove(0);
    }

    /**
     * @brief Sort from list[i] to list[j] (inclusive)
     * 
     * @param i index i
     * @param j index j
     */
    void sort(int i, int j)
    {
        if (i > j)
            return;
        if (out_of_date)
            update();
        int l = i, r = j;
        int mid = l + (l + r) % (r - l + 1);
        while (l < r)
        {
            while (l < mid && *array[l]->p_data < *array[mid]->p_data)
                l++;
            while (r > mid && !(*array[r]->p_data < *array[mid]->p_data))
                r--;
            T *t = array[l]->p_data;
            array[l]->p_data = array[r]->p_data;
            array[r]->p_data = t;
            if (l == mid)
                mid = r;
            else if (r == mid)
                mid = l;
        }
        sort(i, mid - 1);
        sort(mid + 1, j);
    }

    /**
     * @brief Binary search for first element with value x.
     *        Sorted (and updated) required
     * 
     * @return index of value, -1 if not found
     */
    int bsearch(T x) const
    {
        int l = 0, r = size() - 1;
        while (l + 1 < r)
        {
            int m = (l + r) / 2;
            if (*array[m]->p_data < x)
                l = m;
            else if (*array[m]->p_data >= x)
                r = m;
        }
        if (*array[l]->p_data == x)
            return l;
        if (*array[r]->p_data == x)
            return r;
        return -1;
    }

    /**
     * @brief Reduce a multiset to a set when this list is considered as a multiset
     * 
     */
    void set_reduce()
    {
        sort(0, size() - 1);
        llist<T> *p = head.next;
        while (p != NULL)
        {
            llist<T> *q = p->next;
            if (q != NULL && *q->p_data == *p->p_data)
                remove(q);
            else
                p = p->next;
        }
    }

    /**
     * @brief Append b directly after this object, and as a result,
     *        *pb will be cleared
     * 
     * @param pb pointer to another object
     */
    void merge(list<T> *pb)
    {
        if (pb->head.next == NULL || pb == this)
            return;
        p_rear->next = pb->head.next;
        pb->head.next->pre = p_rear;
        p_rear = pb->p_rear;
        pb->head.next = NULL;
        pb->p_rear = &pb->head;
        *(int *)head.p_data += *(int *)pb->head.p_data;
        *(int *)pb->head.p_data = 0;
        out_of_date = true;
        pb->out_of_date = true;
    }

    /**
     * @brief Get the idx-th element
     * 
     * @param idx the index
     * @return the idx-th element
     */
    T &operator[](int idx)
    {
        if (out_of_date)
            update();
        return *array[idx]->p_data;
    }

    /**
     * @brief Assignment function
     * 
     * @param b 
     * @return list<T>& 
     */
    list<T> &operator=(const list<T> &b)
    {
        destroy();
        copy(b);
        return *this;
    }

    /**
     * @brief Append an element and return a new object
     * 
     * @param b the element to append
     * @return the result
     */
    list<T> operator+(const T &b) const
    {
        list<T> c(*this);
        c.append(b);
        return c;
    }

    /**
     * @brief merge two list
     * 
     * @param b another list
     * @return the result
     */
    list<T> operator+(const list<T> &b) const
    {
        list<T> c(*this), d(b);
        for (int i = 0; i < d.size(); i++)
            c.append(d[i]);
        return c;
    }

    /**
     * @brief Append an element
     * 
     * @param b the element to append
     * @return reference of this object
     */
    list<T> &operator+=(const T &b)
    {
        append(b);
        return *this;
    }

    /**
     * @brief Append b to this
     * 
     * @param b another list
     * @return reference to this object
     */
    list<T> &operator+=(const list<T> &b)
    {
        list<T> c(b);
        for (int i = 0; i < c.size(); i++)
            append(c[i]);
        return *this;
    }

    /**
     * @brief Operator ==
     * 
     * @param b another object
     * @return whether this object equals to b
     */
    bool operator==(const list<T> &b) const
    {
        if (size() != b.size())
            return false;
        llist<T> *p = head.next, *bp = b.head.next;
        while (p != NULL && bp != NULL)
        {
            if (*p->p_data != *bp->p_data)
                return false;
            p = p->next;
            bp = bp->next;
        }
        return true;
    }

    /**
     * @brief Operator <
     * 
     * @param b another object
     * @return whether this object is superior than b in dictionary order
     */
    bool operator<(const list<T> &b) const
    {
        llist<T> *p = head.next, *bp = b.head.next;
        while (p != NULL && bp != NULL)
        {
            if (*p->p_data < *bp->p_data)
                return true;
            else if (*p->p_data > *bp->p_data)
                return false;
            p = p->next;
            bp = bp->next;
        }
        if (p == NULL && bp != NULL)
            return true;
        return false;
    }
};
