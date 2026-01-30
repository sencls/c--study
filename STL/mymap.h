#pragma once
#include <iostream>
#include <exception>
#include <utility>
#include <stack>
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
};