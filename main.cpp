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
    if (s.empty())
        return;
    
    int j = 1;
    int i = 0;

    while (j < s.size())
    {
        if (s[j] == s[i])
        {
            pattern[j] = i + 1;
            ++j;
            ++i;
        }
        else
        {
            if(i)
              i = pattern[i - 1];
            else
            {
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
        if (haystack[i] == needle[matched_pos])
        {
            if(matched_pos == n_len - 1)
                return i - n_len + 1;
            ++i;
            ++matched_pos;
        }
        else
        {
            if(matched_pos)
            {
                matched_pos = v[matched_pos -1];
            }
            else
            {
                ++i;
            }
        }
    }
    return -1;
}
int main(void)
{
    string h = "mississippi";
    string n = "issip";
    auto pos = strStrKMP(h, n);

    getchar();
    return 0;
}
