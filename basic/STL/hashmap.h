#pragma once

#include <utility>
#include <vector>
#include <list>
#include <iterator>
#include <stdexcept>
template <typename Keytype, typename Valuetype>
struct hashnode
{
    std::pair<Keytype, Valuetype> data;
    hashnode *next;
    hashnode(const Keytype &k, const Valuetype &val, hashnode<Keytype, Valuetype> *n = nullptr) : data(std::make_pair(k, val)), next(n) {}
};

template <typename Keytype, typename Valuetype, typename Hash = std::hash<Keytype>>
class hashmap
{

public:
    class Iterator;
    using key_type = Keytype;
    using mapped_type = Valuetype;
    using value_type = std::pair<Keytype, Valuetype>;
    using size_type = std::size_t;

    hashmap(size_type initial_capacity = 16, double max_load_factor = 0.75) : bucket_count_(initial_capacity), element_count_(0), max_load_factor_(max_load_factor), hash_func_(Hash()) { bucket_.resize(bucket_count_, nullptr); }
    ~hashmap() { clear(); }
    hashmap(const hashmap &other) = delete;
    hashmap &operator=(const hashmap &other) = delete;
    void insert(const Keytype &k, const Valuetype &val)
    {
        int flag = 1;
        size_type location = hash_func_(k) % bucket_count_;
        hashnode<Keytype, Valuetype> *cur = bucket_[location];
        if (cur == nullptr)
        {
            bucket_[location] = new hashnode(k, val);
            element_count_++;
            flag = 0;
            std::cout << "slot插入" << location << std::endl;
            auto load = static_cast<double>(element_count_) / bucket_count_;
            if (load > max_load_factor_)
                rehash();
            return;
        }
        while (cur->next != nullptr)
        {
            if (cur->data.first == k)
            {
                std::cout << "值修改" << location << std::endl;
                cur->data.second = val;
                return;
            }
            cur = cur->next;
        }
        cur->next = new hashnode(k, val);
        element_count_++;
        if (flag)
            std::cout << "尾修改" << location << std::endl;
        auto load = static_cast<double>(element_count_) / bucket_count_;
        if (load > max_load_factor_)
            rehash();
    }
    value_type *find(const Keytype &key)
    {
        auto location = hash_func_(key) % bucket_count_;
        auto *t = bucket_[location];
        while (t != nullptr)
        {
            if (t->data.first == key)
            {
                return &(t->data);
            }
            t = t->next;
        }
        return nullptr;
    }
    bool erase(const Keytype &k)
    {
        auto location = hash_func_(k) % bucket_count_;
        auto *t = bucket_[location];
        if (t->data.first == k)
        {
            auto *p = t;
            t = t->next;
            bucket_[location] = t;
            delete p;
            element_count_--;
            return true;
        }
        while (t->next != nullptr)
        {
            if (t->next->data.first == k)
            {
                auto *p = t->next;
                t->next = t->next->next;
                element_count_--;
                delete p;
            }
            t = t->next;
        }
        return false;
    }
    size_type size() const
    {
        return element_count_;
    }
    void clear()
    {
        for (size_type i = 0; i < bucket_.size(); i++)
        {
            hashnode<Keytype, Valuetype> *cur = bucket_[i];
            while (cur != nullptr)
            {
                hashnode<Keytype, Valuetype> *t = cur;
                cur = cur->next;
                delete t;
            }
            bucket_[i] = nullptr;
        }
        element_count_ = 0;
    }
    Iterator begin()
    {
        for (size_type i = 0; i < bucket_count_; i++)
        {
            if (bucket_[i] != nullptr)
            {
                return Iterator(this, i, bucket_[i]);
            }
        }
        return end();
    }
    Iterator end()
    {
        return Iterator(this, bucket_count_, nullptr);
    }
    class Iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = std::pair<Keytype, Valuetype>;
        using difference_type = std::ptrdiff_t;
        using reference = value_type &;
        using pointer = value_type *;
        Iterator(hashmap *map, size_type bucket_index, hashnode<Keytype, Valuetype> *node) : map_(map), bucket_index_(bucket_index), curr(node) {}
        reference operator*() const
        {
            if (curr == nullptr)
            {
                throw std::out_of_range("Iterator *");
            }
            return curr->data;
        }
        pointer operator->() const
        {
            if (curr == nullptr)
            {
                throw std::out_of_range("Iterator ->");
            }
            return &(curr->data);
        }
        Iterator &operator++()
        {
            advance();
            return *this;
        }
        Iterator operator++(int)
        {
            Iterator t = *this;
            advance();
            return t;
        }

        bool operator==(const Iterator &other) const
        {
            return map_ == other.map_ && bucket_index_ == other.bucket_index_ &&
                   curr == other.curr;
        }
        bool operator!=(const Iterator &other) const
        {
            return !(*this == other);
        }

    private:
        hashmap *map_;
        size_type bucket_index_;
        hashnode<Keytype, Valuetype> *curr;
        void advance()
        {
            if (curr != nullptr)
            {
                curr = curr->next;
            }
            while (curr == nullptr && (bucket_index_ < map_->bucket_count_))
            {
                ++bucket_index_;
                curr = bucket_index_ == map_->bucket_count_ ? nullptr : map_->bucket_[bucket_index_];
            }
        }
    };

private:
    size_type bucket_count_;
    size_type element_count_;
    double max_load_factor_;
    std::vector<hashnode<Keytype, Valuetype> *> bucket_;
    Hash hash_func_;
    void rehash()
    {
        auto new_buck = bucket_count_ * 2;
        std::vector<hashnode<Keytype, Valuetype> *> nre(new_buck, nullptr);
        for (size_type i = 0; i < bucket_.size(); i++)
        {
            auto t = bucket_[i];
            while (t != nullptr)
            {
                auto *back = t->next;
                auto new_l = hash_func_(t->data.first) % new_buck;
                t->next = nre[new_l];
                nre[new_l] = t;
                t = back;
            }
        }
        bucket_ = std::move(nre);
        bucket_count_ = new_buck;
    }
};
