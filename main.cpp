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
                i = i - pattern[i];
            }
        }
    }
}

int strStr(string haystack, string needle)
{
    auto h_len = haystack.size();
    auto n_len = needle.size();
    if (n_len > h_len)
        return -1;
    if (n_len == 0)
        return 0;

    vector<int> v(n_len, 0);

    GetSkipPattern(needle, v);

    for (int i = 0; i < h_len;)
    {
        if (haystack[i] == needle[0])
        {
            int j = 1;
            for (; j < n_len; ++j)
            {
                if (needle[j] != haystack[i + j])
                {
                    break;
                }
            }
            if (j == n_len)
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
