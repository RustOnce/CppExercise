#include <vector>
#include <iostream>
#include <string>
#include <future>
#include <chrono>
#include <assert.h>
#include <thread>

using namespace std;

typedef struct TreeNode
{
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x):val(x), left(nullptr), right(nullptr) {};
}TreeNode;

TreeNode *BuildBST();
void DeStructTree(TreeNode *root);

int main(void)
{
    auto root = BuildBST();
    getchar();
    return 0;
}

TreeNode *BuildBST()
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
    node->right= new TreeNode(100);
    node->right->left = new TreeNode(90);
    node->right->right = new TreeNode(130);

    return root;
}

void DestructTree(TreeNode *root)
{
    if(!root)
        return;

    if(root->left)
        DestructTree(root->left);
    if(root->right)
        DestructTree(root->right);
    
    delete root;
}