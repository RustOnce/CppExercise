#ifndef _RUST_CPP_EXERCISE_BINARY_TREE_H_
#define _RUST_CPP_EXERCISE_BINARY_TREE_H_

#include <iostream>
#include <stack>

typedef struct TreeNode
{
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x)
        : val(x)
        , left(nullptr)
        , right(nullptr){};
} TreeNode;

void PreOrderTraverse(TreeNode* root)
{
    if (!root)
        return;
    std::stack<TreeNode*> s;
    auto node = root;
    s.push(node);
    while (!s.empty())
    {
        node = s.top();
        std::cout << node->val << " ";
        s.pop();
        if (node->right)
            s.push(node->right);
        if (node->left)
            s.push(node->left);
    }
}

void InOrderTraverse(TreeNode* root) {}

void PostOrderTraverse(TreeNode* root) {}

TreeNode* BuildBST()
{
    auto root = new TreeNode(56);
    auto node = new TreeNode(28);
    root->left = node;
    auto temp = new TreeNode(20);
    node->left = temp;
    temp->right = new TreeNode(26);
    root->left->right = new TreeNode(38);
    root->right = new TreeNode(75);
    node = root->right;
    node->left = new TreeNode(62);
    node->right = new TreeNode(100);
    node->right->left = new TreeNode(90);
    node->right->right = new TreeNode(130);

    return root;
}

void DestructTree(TreeNode* root)
{
    if (!root)
        return;

    if (root->left)
        DestructTree(root->left);
    if (root->right)
        DestructTree(root->right);

    delete root;
}

#endif