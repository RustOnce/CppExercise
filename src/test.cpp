#include "test.h"
#include <iostream>

const int g_i = 0;

void PrintGlobal()
{
    //std::cout << &g_i << endl;
}

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

void GetSkipPattern2(const string &s, vector<int> &pattern)
{
    if (s.empty())
        return;
    
    int j = 0;
    int i = 0;

    while(++i < s.size())
    {
        int temp = pattern[j];
        if(s[i] == s[temp])
        {
            pattern[i] = temp + 1;
            j = i;
        }
        else
        {
            if(temp == 0)
            {
                pattern[i] = 0;
                j = i;
            }
            else
            {
                j = temp - 1;
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
    vector<int> v2(n_len, 0);

    GetSkipPattern(needle, v);
    GetSkipPattern2(needle, v2);
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

static void staticFunc()
{

}