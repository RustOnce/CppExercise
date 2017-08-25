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

void InOrderTraverse(TreeNode* root)
{
    if (!root)
        return;

    std::stack<TreeNode*> s;
    auto node = root;
    while (!s.empty() || node)
    {
        if (!node)
        {
            node = s.top();
            std::cout << node->val << " ";
            s.pop();
            node = node->right;
            continue;
        }

        s.push(node);
        node = node->left;
    }
}

void PostOrderTraverse(TreeNode* root)
{
    if (!root)
        return;

    std::stack<TreeNode*> s;
    auto node = root;
    TreeNode* last_node = nullptr;
    s.push(node);
    while (!s.empty())
    {
        if (node->left == nullptr && node->right == nullptr || (last_node && (node->left == last_node || node->right == last_node)))
        {
            s.pop();
            std::cout << node->val << " ";
            last_node = node;
            node = nullptr;
        }
        else
        {
            if (node->right)
                s.push(node->right);
            if (node->left)
                s.push(node->left);
        }
        node = s.top();
    }
}

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

void UpdateRightNodeVal(TreeNode* node, int father_val, int father_origin);

inline void UpdateLeftNodeVal(TreeNode* node, int father_val)
{
    node->val += father_val;
    if (node->left)
    {
        node->val -= node->left->val;
        UpdateLeftNodeVal(node->left, node->val);
    }
    if (node->right)
    {
        auto val = node->val - father_val - node->right->val;
        UpdateRightNodeVal(node->right, node->val, val);
    }
}
inline void UpdateRightNodeVal(TreeNode* node, int father_val, int father_origin)
{
    int val = node->val;
    father_val -= father_origin;
    if (node->left)
    {
        father_val -= node->left->val;
        val -= node->left->val;
        UpdateLeftNodeVal(node->left, father_val);
    }
    if (node->right)
    {
        val -= node->right->val;
        UpdateRightNodeVal(node->right, father_val, val);
    }
}
TreeNode* convertBST(TreeNode* root)
{
    if (!root)
        return root;

    std::stack<TreeNode*> s;
    auto node = root;
    TreeNode* last_node = nullptr;
    s.push(node);
    while (!s.empty())
    {
        node = s.top();
        if (node->left == nullptr && node->right == nullptr || (last_node && (node->left == last_node || node->right == last_node)))
        {
            s.pop();
            if (node->left)
                node->val += node->left->val;
            if (node->right)
                node->val += node->right->val;
            last_node = node;
            node = nullptr;
        }
        else
        {
            if (node->right)
                s.push(node->right);
            if (node->left)
                s.push(node->left);
        }
    }

    if (root->left)
    {
        root->val -= root->left->val;
        UpdateLeftNodeVal(root->left, root->val);
    }
    if (root->right)
    {
        auto val = root->val - root->right->val;
        UpdateRightNodeVal(root->right, root->val, val);
    }

    return root;
}

#endif