#pragma once
#include <iostream>

template <typename T>
struct Node
{
    T data;
    Node *next;
    Node *prev;
    Node(const T &data = T()) : data(data), next(nullptr), prev(nullptr) {}
};

template <typename T>
class List;

template <typename T>
class Iterator
{
    Node<T> *node_ptr;
    friend class List<T>;

public:
    using self_type = Iterator<T>;
    using value_type = T;
    using reference = T &;
    using pointer = T *;
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;

    Iterator(Node<T> *ptr = nullptr) : node_ptr(ptr) {}
    reference operator*() const
    {
        return node_ptr->data;
    }
    pointer operator->() const
    {
        return &(node_ptr->data);
    }
    self_type &operator++()
    {
        if (node_ptr->next)
            node_ptr = node_ptr->next;
        return *this;
    }
    self_type operator++(int)
    {
        self_type temp = *this;
        ++(*this);
        return temp;
    }
    self_type &operator--()
    {
        if (node_ptr->prev)
            node_ptr = node_ptr->prev;
        return *this;
    }
    self_type operator--(int)
    {
        self_type temp = *this;
        --(*this);
        return temp;
    }
    bool operator==(const self_type &other) const
    {
        return node_ptr == other.node_ptr;
    }
    bool operator!=(const self_type &other) const
    {
        return node_ptr != other.node_ptr;
    }
};
template <typename T>
class List
{
private:
    Node<T> *head;
    Node<T> *tail;

public:
    using iterator = Iterator<T>;
    using const_iterator = Iterator<T>;
    List()
    {
        head = new Node<T>();
        tail = new Node<T>();
        head->next = tail;
        tail->prev = head;
    }
    ~List()
    {
        clear();
        delete head;
        delete tail;
    }
    List(const List &other) = delete;
    List &operator=(const List &other) = delete;
    void clear()
    {
        Node<T> *cur = head->next;
        while (cur != tail)
        {
            Node<T> *next = cur->next;
            delete cur;
            cur = next;
        }
        head->next = tail;
        tail->prev = head;
    }
    iterator insert(iterator it, const T &val)
    {
        Node<T> *old = it.node_ptr;
        Node<T> *old_prev = old->prev;
        Node<T> *newp = new Node<T>(val);
        old_prev->next = newp;
        newp->prev = old_prev;
        newp->next = old;
        old->prev = newp;
        return iterator(newp);
    }
    iterator erase(iterator pos)
    {
        Node<T> *cur = pos.node_ptr;
        if (cur == head || cur == tail)
            return iterator(tail);
        Node<T> *prev = cur->prev;
        Node<T> *next = cur->next;
        prev->next = next;
        next->prev = prev;
        delete cur;
        return iterator(next);
    }
    void push_front(const T &val)
    {
        insert(begin(), val);
    }
    void push_back(const T &val)
    {
        insert(end(), val);
    }
    iterator begin()
    {
        return iterator(head->next);
    }
    iterator end()
    {
        return iterator(tail);
    }
    void pop_front()
    {
        if (!empty())
            erase(begin());
    }
    bool empty() const
    {
        return head->next == tail;
    }
    void pop_back()
    {
        if (!empty())
        {
            iterator temp = end();
            --temp;
            erase(temp);
        }
    }
    T &front()
    {
        return head->next->data;
    }
    T &back()
    {
        return tail->prev->data;
    }
    size_t size() const
    {
        size_t cnt = 0;
        for (auto &it = begin(); it != end(); ++it)
        {
            cnt++;
        }
        return cnt;
    }
    void print() const
    {
        Node<T> *cur = head->next;
        while (cur != tail)
        {
            std::cout << cur->data << " ";
            cur = cur->next;
        }
    }
};
