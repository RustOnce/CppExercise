#include <future>
#include <iostream>
#include <stack>
#include <string>
#include <thread>
#include <vector>

using namespace std;

void PrintGlobal();

void GetPatterns2(string s);
void GetSkipPattern(const string &s, vector<int> &pattern);
void GetSkipPattern2(const string &s, vector<int> &pattern);
int strStrKMP(string haystack, string needle);