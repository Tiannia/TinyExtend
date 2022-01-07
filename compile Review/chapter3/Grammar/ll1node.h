#ifndef LL1NODE_H
#define LL1NODE_H
#include <bits/stdc++.h>

using namespace std;

struct LL1Node
{
    //string left;
    //vector<vector<int>> right;
    //如果是单个字符，可以直接用
    char left;
    vector<string> right;
    LL1Node(char l) : left(l) {}
    void insert(string str)
    {
        right.push_back(str);
    }

    void insert(vector<string>::iterator st, vector<string>::iterator end)
    {
        for (auto it = st; it != end; ++it)
        {
            right.push_back(*it);
        }
    }
};

#endif