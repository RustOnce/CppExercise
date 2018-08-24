#include "./common/BinaryTree.hpp"
#include "test.h"
#include <chrono>
#include <future>
#include <iostream>
#include <stack>
#include <string>
#include <thread>
#include <vector>
#include <locale>
#include <map>

// 64bcae05043280b5f2d278944f53dd94004ae8db
//0b64cb4059809ab854d9e4383ca4c1dc
using namespace std;

class Foo
{
public:
    Foo()
    {
        std::cout << "Foo::Foo()" << endl;
    }
    void f()
    {
        std::cout << "Foo::f()" << endl;
    }
};

class Foo1 : public Foo
{
public:
    void f()
    {
        std::cout << "Foo1:f()" << endl;
    }
};

class A
{
public:
    void Action()
    {
        m_foo.f();
    }

    Foo m_foo;
};

class B : public A
{
public:
    Foo1 m_foo;
};

int main(void)
{
    std::map<int, int> map;
    int i = 12345;
    float f = 12345;
    std::cout << f << endl;
    
    getchar();
    return 0;
}
