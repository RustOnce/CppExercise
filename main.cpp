#include "./common/BinaryTree.hpp"
#include <chrono>
#include <future>
#include <iostream>
#include <stack>
#include <string>
#include <thread>
#include <vector>

// 2dbe109240b13818635973599c88bb58

using namespace std;

int main(void)
{
    auto root = BuildBST();

    InOrderTraverse(root);
    getchar();
    return 0;
}
