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
void GetSkipPattern(const string &s, vector<int> &pattern)
{
    if (s.empty())
        return;
    pattern[0] = 1;
    int j = 1;
    int i = 0;

    while (j < s.size())
    {
        if (s[j] == s[i - pattern[i] + 1])
        {
            pattern[j] = pattern[i];
            i = j++;
        }
        else
        {
            if (pattern[i] == i + 1)
            {
                pattern[j] = j + 1;
                i = j++;
            }
            else
            {
                i = pattern[i] - 1;
            }
        }
    }
}

int strStr(string haystack, string needle)
{
    if (needle.size() > haystack.size())
        return -1;
    if (needle.empty() || haystack == needle)
        return 0;

    vector<int> v(needle.size(), 0);
    GetSkipPattern(needle, v);

    for (int i = 0; i < haystack.size();)
    {
        if (haystack[i] == needle[0])
        {
            int j = 1;
            for (; j < needle.size(); ++j)
            {
                if (needle[j] != haystack[i + j])
                {
                    break;
                }
            }
            if (j == needle.size())
                return i;
            else
            {
                i += v[j - 1];
            }
        }
        else
        {
            ++i;
        }
    }
    return -1;
}
int main(void)
{
    string h = "ababcaababcaabc";
    string n = "ababcaabc";
    auto pos = strStr(h, n);

    getchar();
    return 0;
}
