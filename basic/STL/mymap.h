#pragma once
#include <iostream>
#include <exception>
#include <utility>
#include <stack> //基于BST
template <typename key, typename value>
struct TreeNode
{
    std::pair<key, value> data;
    TreeNode *left;
    TreeNode *right;
    TreeNode *parent;
    TreeNode(const key &k, const value &val, TreeNode *par = nullptr) : data(std::make_pair(k, val)), left(nullptr), right(nullptr), parent(par) {}
};

template <typename key, typename value>
class mymap
{

    TreeNode<key, value> *root;
    void del(TreeNode<key, value> *node)
    {
        if (node == nullptr)
            return;
        del(node->left);
        del(node->right);
        delete node;
    }
    void preorder(TreeNode<key, value> *node) const
    {
        if (node == nullptr)
            return;
        preorder(node->left);
        std::cout << node->data.first << " " << node->data.second << std::endl;
        preorder(node->right);
    }
    void helper1(TreeNode<key, value> *&node, const key &k, const value &val)
    {
        if (node == nullptr)
            node = new TreeNode<key, value>(k, val);
        else if (k < node->data.first)
        {
            if (!node->left)
            {
                node->left = new TreeNode<key, value>(k, val);
                node->left->parent = node;
            }

            else
                helper1(node->left, k, val);
        }
        else
        {
            if (!node->right)
            {
                node->right = new TreeNode<key, value>(k, val);
                node->right->parent = node;
            }

            else
                helper1(node->right, k, val);
        }
    }
    TreeNode<key, value> *maxn(TreeNode<key, value> *node) const
    {
        while (node->right != nullptr)
        {
            node = node->right;
        }
        return node;
    }
    TreeNode<key, value> *minn(TreeNode<key, value> *node) const
    {
        while (node->left != nullptr)
        {
            node = node->left;
        }
        return node;
    }
    TreeNode<key, value> *successor(TreeNode<key, value> *node) const
    {
        if (node->right != nullptr)
        {
            return minn(node->right);
        }

        auto *p = node->parent;
        while (p != nullptr && node == p->right)
        {
            node = p;
            p = p->parent;
        }

        return p;
    }

public:
    mymap() : root(nullptr) {}
    ~mymap()
    {
        del(root);
    }
    mymap(const mymap<key, value> &other) = delete;
    mymap<key, value> &operator=(const mymap<key, value> &other) = delete;
    void print() const
    {
        preorder(root);
    }
    void insert(const key &k, const value &val)
    {
        helper1(this->root, k, val);
    }
    void clear()
    {
        del(root);
        root = nullptr;
    }

    void erase(const key &k)
    {
        auto *node = find(k);
        if (node == nullptr)
            return;
        if (node->left && node->right)
        {
            auto *su = minn(node->right);
            node->data = su->data;
            node = su;
        }

        auto *chile = (node->right) ? node->left : node->right;
        if (chile != nullptr)
            chile->parent = node->parent;

        if (node->parent == nullptr)
        {
            root = chile;
        }
        else if (node == node->parent->left)
        {
            node->parent->left = chile;
        }
        else
        {
            node->parent->right = chile;
        }
        delete node;
    }

    TreeNode<key, value> *find(const key &k) const
    {
        auto *curr = root;
        while (curr != nullptr)
        {
            if (k < curr->data.first)
            {
                curr = curr->left;
            }
            else if (k > curr->data.first)
                curr = curr->right;
            else
                return curr;
        }
    }

    class Iterator
    {
        TreeNode<key, value> *cur;
        TreeNode<key, value> *maxn(TreeNode<key, value> *node) const
        {
            while (node->right != nullptr)
            {
                node = node->right;
            }
            return node;
        }
        TreeNode<key, value> *minn(TreeNode<key, value> *node) const
        {
            while (node->left != nullptr)
            {
                node = node->left;
            }
            return node;
        }
        TreeNode<key, value> *successor(TreeNode<key, value> *node) const
        {
            if (node->right != nullptr)
            {
                return minn(node->right);
            }

            auto *p = node->parent;
            while (p != nullptr && node == p->right)
            {
                node = p;
                p = p->parent;
            }

            return p;
        }

    public:
        Iterator(TreeNode<key, value> *node) : cur(node) {}
        std::pair<key, value> &operator*() const { return cur->data; }
        std::pair<key, value> *operator->() const { return &cur->data; }
        bool operator==(const Iterator &other) const { return cur == other.cur; }
        bool operator!=(const Iterator &other) const { return !(*this == other); }
        Iterator &operator++()
        {
            if (cur != nullptr)
            {
                cur = successor(cur);
            }
            return *this;
        }
        Iterator operator++(int)
        {
            Iterator t = *this;
            ++this;
            return t;
        }
    };
    Iterator begin() const
    {
        return Iterator(minn(root));
    }
    Iterator end() const
    {
        return Iterator(nullptr);
    }
};