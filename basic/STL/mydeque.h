#pragma once

#include <iostream>

template <typename T>
class Deque
{
private:
    T *buffer;
    size_t capacity;
    size_t count;
    size_t front_idx;
    size_t back_idx;

public:
    Deque(size_t initial_capacity = 10) : capacity(initial_capacity), count(0), front_idx(0), back_idx(0)
    {
        buffer = new T[capacity];
    }
    ~Deque()
    {
        delete[] buffer;
    }
    bool empty() const
    {
        return count == 0;
    }
    size_t size() const
    {
        return count;
    }
    void resize(size_t new_cap)
    {
        T *n_buffer = new T[new_cap];
        for (size_t i = 0; i < count; i++)
        {
            n_buffer[i] = buffer[(front_idx + i) % capacity];
        }
        delete[] buffer;
        front_idx = 0;
        back_idx = count;
        buffer = n_buffer;
        capacity = new_cap;
    }
    void push_front(const T &val)
    {
        if (count == capacity)
        {
            resize(capacity * 2);
        }
        front_idx = (front_idx - 1 + capacity) % capacity;
        buffer[front_idx] = val;
        count++;
    }
    void push_back(const T &val)
    {
        if (count == capacity)
        {
            resize(capacity * 2);
        }
        buffer[back_idx] = val;
        back_idx = (back_idx + 1) % capacity;
        count++;
    }
    void pop_front()
    {
        if (count == 0)
        {
            throw std::out_of_range("empty");
        }
        front_idx = (front_idx + 1) % capacity;
        count--;
    }
    void pop_back()
    {
        if (count == 0)
        {
            throw std::out_of_range("empty");
        }

        back_idx = (back_idx == 0 ? capacity - 1 : back_idx - 1);
        count--;
    }
    T &back()
    {
        if (empty())
            throw std::out_of_range("empty");
        size_t t = back_idx == 0 ? capacity - 1 : back_idx - 1;
        return buffer[t];
    }
    const T &back() const
    {
        if (empty())
            throw std::out_of_range("empty");
        size_t t = back_idx == 0 ? capacity - 1 : back_idx - 1;
        return buffer[t];
    }
    T &front()
    {
        if (empty())
            throw std::out_of_range("empty");
        return buffer[front_idx];
    }
    const T &front() const
    {
        if (empty())
            throw std::out_of_range("empty");
        return buffer[front_idx];
    }
    class Iterator
    {
    private:
        Deque<T> *deque_ptr;
        size_t pos;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference = std::ptrdiff_t;
        using pointer = T *;
        using reference = T &;

        Iterator(Deque<T> *deque, size_t pos) : deque_ptr(deque), pos(pos) {}
        reference operator*() const
        {
            size_t t = (deque_ptr->front_idx + pos) % deque_ptr->capacity;
            return deque_ptr->buffer[t];
        }
        pointer operator->() const
        {
            size_t t = (deque_ptr->front_idx + pos) % deque_ptr->capacity;
            return &(deque_ptr->buffer[t]);
        }
        Iterator &operator++()
        {
            ++pos;
            return *this;
        }
        Iterator &operator++(int)
        {
            Iterator t = *this;
            ++pos;
            return t;
        }
        Iterator &operator--()
        {
            --pos;
            return *this;
        }
        Iterator &operator--(int)
        {
            Iterator t = *this;
            --pos;
            return t;
        }
        bool operator==(const Iterator &other)
        {
            return (deque_ptr == other.deque_ptr && pos == other.pos);
        }
        bool operator!=(const Iterator &other)
        {
            return !(*this == other);
        }
    };
    Iterator begin()
    {
        return Iterator(this, 0);
    }
    Iterator end()
    {
        return Iterator(this, count);
    }
};