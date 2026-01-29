#pragma once

struct ControlBlock
{
    int ref_count;
    ControlBlock() : ref_count(1) {}
};

template <typename T>
class simpleSharedPtr
{
private:
    T *ptr;
    ControlBlock *control;
    void release()
    {
        if (control)
        {
            --control->ref_count;
            if (control->ref_count == 0)
            {
                delete ptr;
                ptr = nullptr;
                delete control;
                control = nullptr;
            }
        }
    }

public:
    simpleSharedPtr() : ptr(nullptr), control(nullptr) {}
    explicit simpleSharedPtr(T *p) : ptr(p) // 显式调用这个构造函数
    {
        if (p)
        {
            control = new ControlBlock();
        }
        else
            control = nullptr;
    }
    ~simpleSharedPtr()
    {
        if (ptr)
            release();
    }
    simpleSharedPtr(const simpleSharedPtr &a) : ptr(a.ptr), control(a.control) // 拷贝构造
    {
        if (control)
            ++control->ref_count;
    }
    simpleSharedPtr &operator=(const simpleSharedPtr &a) // 拷贝赋值
    {
        if (this != &a)
        {
            release();
            ptr = a.ptr;
            control = a.control;
            if (control)
                ++control->ref_count;
        }
        return *this;
    }
    simpleSharedPtr(simpleSharedPtr &&other) noexcept : ptr(other.ptr), control(other.control) // 移动构造
    {
        other.ptr = nullptr;
        other.control = nullptr;
    }
    simpleSharedPtr &operator=(simpleSharedPtr &&other) noexcept
    {
        if (this != &other)
        {
            release();
            ptr = other.ptr;
            control = other.control;
            other.ptr = nullptr;
            other.control = nullptr;
        }
        return *this;
    }
    T *operator->() const
    {
        return ptr;
    }

    T &operator*() const
    {
        return *ptr;
    }

    T *get() const
    {
        return ptr;
    }

    int use_count() const
    {
        return control ? control->ref_count : 0;
    }

    void reset(T *p = nullptr)
    {
        release();
        ptr = p;
        if (p)
        {
            control = new ControlBlock();
        }
        else
        {
            control = nullptr;
        }
    }
};
