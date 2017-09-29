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

void GetPatterns2(string s)
{
    int nsize = s.size();
    vector<int> table(nsize, 0);
    //building match table
    for (int i = 1, j = 0; i < nsize - 1;){
        if (s[i] != s[j]){
            if (j>0){
                j = table[j - 1];
            }
            else{
                i++;
            }
        }
        else{
            table[i] = j + 1;
            i++;
            j++;
        }
    }
}

void GetSkipPattern(const string &s, vector<int> &pattern)
{
    GetPatterns2(s);
    if (s.empty())
        return;
    pattern[0] = 1;
    int j = 1;
    int i = 0;

    while (j < s.size())
    {
        if (s[j] == s[i])
        {
            pattern[j] = j - i;
            ++j;
            ++i;
        }
        else
        {
            if(i)
              i = i - pattern[i - 1];
            else
            {
                pattern[j] = j + 1;
                ++j;
            }
        }
    }
}

int strStrKMP(string haystack, string needle)
{
    auto h_len = haystack.size();
    auto n_len = needle.size();
    if (n_len > h_len)
        return -1;
    if (n_len == 0)
        return 0;

    vector<int> v(n_len, 0);

    GetSkipPattern(needle, v);
    int matched_pos = 0;
    for (int i = 0; i < h_len;)
    {
        if (haystack[i] == needle[i - matched_pos])
        {
            if(i - matched_pos == n_len - 1)
                return matched_pos;
            ++i;
        }
        else
        {
            if(i == matched_pos)
            {
                matched_pos = ++i;
            }
            else
            {
                matched_pos += v[i - matched_pos - 1];     
            }
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
