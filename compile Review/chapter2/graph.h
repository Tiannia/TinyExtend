#ifndef GRAPH_H
#define GRAPH_H

#define MAXVER 1000
#include <bits/stdc++.h>
using namespace std;

class Edge
{
public:
    char element;
    int to;
    Edge *nxt;
    Edge() : element('~'), to(-1), nxt(nullptr) {}
    Edge(char ele, int to) : element(ele), to(to), nxt(nullptr) {}
};

class Vertex
{

public:
    int val;
    Vertex *nxt;
    Edge *link;
    Vertex() : val(-1), nxt(nullptr), link(nullptr) {}
    Vertex(int num) : val(num), nxt(nullptr), link(nullptr) {}
};

class graph
{
private:
    /* data */
    Vertex *NodeTable;
    int numofvertices;
    int numofedges;

public:
    graph() : numofvertices(0), numofedges(0) { NodeTable = new Vertex[MAXVER]; }
    ~graph() { Gclear(); }

    void Gclear();
    void InsertVertex(int value)
    {
        NodeTable[numofvertices].val = value;
        numofvertices++;
    }

    void InsertEdge(int value1, int value2, char ele)
    {
        int v1 = getPos(value1), v2 = getPos(value2);

        if (v1 >= numofvertices || v2 >= numofvertices)
            return;
        if (v1 < 0 || v2 < 0)
            return;
        Edge *p = NodeTable[v1].link;
        while (p)
        {
            if (p->nxt && p->nxt->to == v2 && p->nxt->element == ele)
                return;
            p = p->nxt;
        }
        //头插法
        p = NodeTable[v1].link;
        NodeTable[v1].link = new Edge(ele, v2);
        NodeTable[v1].link->nxt = p;
    }

    int getValue(int pos)
    {
        return pos >= 0 && pos < numofvertices ? NodeTable[pos].val : -1;
    }

    int getPos(int value)
    {
        for (int i = 0; i < numofvertices; ++i)
        {
            if (NodeTable[i].val == value)
                return i;
        }
        return -1;
    }

    set<int> Closure(set<int> st)
    {
        set<int> res(st);
        vector<int> now(res.begin(), res.end());
        for (auto value : now)
        {
            int pos = getPos(value);
            assert(pos >= 0);
            auto pEdge = NodeTable[pos].link;
            while (pEdge)
            {
                if (pEdge->element == '~')
                {
                    if (!res.count(pEdge->to))
                    {
                        now.push_back(pEdge->to);
                        res.insert(pEdge->to);
                    }
                }
                pEdge = pEdge->nxt;
            }
        }
        return res;
    }

    set<int> Move(set<int> st, char ch)
    {
        set<int> res;
        for (auto it : st)
        {
            int pos = getPos(it);
            assert(pos >= 0);
            auto pEdge = NodeTable[pos].link;
            while (pEdge)
            {
                if (pEdge->element == ch)
                {
                    res.insert(pEdge->to);
                }
                pEdge = pEdge->nxt;
            }
        }
        return res;
    }
};

#endif