// 基于AVL
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
template <typename KeyType, typename ValueType>
struct AVLNode
{
    KeyType key;
    ValueType value;
    int height;
    AVLNode *left;
    AVLNode *right;
    AVLNode(const KeyType &key, const ValueType &value) : key(key), value(value), height(1), left(nullptr), right(nullptr) {}
};

template <typename KeyType, typename ValueType>
class AVLmap
{
    AVLNode<KeyType, ValueType> *root;
    int getheight(AVLNode<KeyType, ValueType> *root) const
    {
        if (root == nullptr)
            return 0;
        return root->height;
    }
    int getBalance(AVLNode<KeyType, ValueType> *root)
    {
        if (root == nullptr)
            return 0;
        return getheight(root->left) - getheight(root->right);
    }
    AVLNode<KeyType, ValueType> *rightRotate(AVLNode<KeyType, ValueType> *y)
    {
        AVLNode<KeyType, ValueType> *x = y->left;
        auto *T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = std::max(getheight(y->left), getheight(y->right)) + 1;
        x->height = std::max(getheight(x->left), getheight(x->right)) + 1;

        return x;
    }
    AVLNode<KeyType, ValueType> *leftRotate(AVLNode<KeyType, ValueType> *x)
    {
        auto *y = x->right;
        auto *T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = std::max(getheight(x->left), getheight(x->right)) + 1;
        y->height = std::max(getheight(y->left), getheight(y->right)) + 1;

        return y;
    }
    void clear(AVLNode<KeyType, ValueType> *node)
    {
        if (node == nullptr)
            return;
        if (node->left)
            clear(node->left);
        if (node->right)
            clear(node->right);
        delete node;
    }
    AVLNode<KeyType, ValueType> *insert(AVLNode<KeyType, ValueType> *node, const KeyType &k, const ValueType &val)
    {
        if (node == nullptr)
            return new AVLNode<KeyType, ValueType>(k, val);

        if (k < node->key)
        {
            node->left = insert(node->left, k, val);
        }
        if (k > node->key)
        {
            node->right = insert(node->right, k, val);
        }
        else
        {
            node->value = val;
            return node;
        }
        node->height = 1 + std::max(getheight(node->left), getheight(node->right));
        int balance = getBalance(node);
        if (balance > 1 && k < node->left->key)
        {
            return rightRotate(node);
        }
        if (balance < -1 && k > node->right->key)
        {
            return leftRotate(node);
        }
        if (balance < -1 && k < node->right->key)
        {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }
        if (balance > 1 && k > node->left->key)
        {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }

        return node;
    }
    AVLNode<KeyType, ValueType> *getMinn(AVLNode<KeyType, ValueType> *node)
    {
        if (node == nullptr)
            return nullptr;
        return getMinn(node->left);
    }
    AVLNode<KeyType, ValueType> *deleteNode(AVLNode<KeyType, ValueType> *root, const KeyType &k)
    {
        if (root == nullptr)
            return nullptr;
        if (k < root->key)
            root->left = deleteNode(root->left, k);
        else if (k > root->key)
            root->right = deleteNode(root->right, k);
        else
        {
            if (!root->left || !root->right)
            {
                auto *temp = root->left ? root->left : root->right;
                if (temp == nullptr)
                {
                    temp = root;
                    root = nullptr;
                }
                else
                {
                    *root = *temp;
                }
                delete temp;
            }
            else
            {
                auto *temp = getMinn(root->right);
                root->key = temp->key;
                root->value = temp->value;
                root->right = deleteNode(root->right, temp->key);
            }
        }

        if (root == nullptr)
            return root;
        root->height = 1 + std::max(getheight(root->left), getheight(root->right));
        int balance = getBalance(root);
        if (balance > 1 && k < root->left->key)
        {
            return rightRotate(root);
        }
        if (balance < -1 && k > root->right->key)
        {
            return leftRotate(root);
        }
        if (balance < -1 && k < root->right->key)
        {
            root->right = rightRotate(root->right);
            return leftRotate(root);
        }
        if (balance > 1 && k > root->left->key)
        {
            root->left = leftRotate(root->left);
            return rightRotate(root);
        }

        return root;
    }
    ValueType *serchNode(AVLNode<KeyType, ValueType> *root, const KeyType &key)
    {
        if (root == nullptr)
            return nullptr;
        ValueType *t;
        if (key < root->key)
        {
            return search(root->left, key);
        }
        else if (key > root->key)
        {
            return search(root->right, key);
        }
        else
            return &(root->value);
    }
    void inorderh(AVLNode<KeyType, ValueType> *root, std::vector<std::pair<KeyType, ValueType>> &a) const
    {
        if (root == nullptr)
            return;
        inorderh(root->left, a);
        a.push_back(std::make_pair(root->key, root->value));
        inorderh(root->right, a);
    }

public:
    AVLmap() : root(nullptr) {}
    ~AVLmap()
    {
        std::function<void(AVLNode<KeyType, ValueType> *)> destory = [&](AVLNode<KeyType, ValueType> *node)
        {
            if (node == nullptr)
                return;
            destory(node->left);
            destory(node->right);
            delete node;
        };
        destory(root);
    }
    void put(const KeyType &k, const ValueType &val)
    {
        root = insert(root, k, val);
    }
    ValueType *get(const KeyType &k)
    {
        return searchNode(root, k);
    }
    void remove(const KeyType &key)
    {
        root = deleteNode(root, key);
    }
    std::vector<std::pair<KeyType, ValueType>> inorder() const
    {
        std::vector<std::pair<KeyType, ValueType>> result;
        inorderh(root, result);
        return result;
    }
};