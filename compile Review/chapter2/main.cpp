#include "graph.h"

//NFA图
graph *NFAGraph;

//NFA状态数
int NFAStatesNumber;

//DFA状态数
int DtranNumber;

//DFA存储结构（状态转换，value对应状态下标）
vector<map<char, int>> Dtran;

//DFA存储结构 (所有状态)
vector<set<int>> DStates;

//接受集合
set<int> AcceptStates;

//转换符集合
set<char> edges;

string exp, post;

void InsertPoint(string &exp)
{
    for (int i = 0; i + 1 < exp.size(); ++i)
    {
        if (((exp[i] != '(' && exp[i] != '.' && exp[i] != '|') ||
             exp[i] == ')' || exp[i] == '*') &&
            (exp[i + 1] != ')' && exp[i + 1] != '.' && exp[i + 1] != '|' && exp[i + 1] != '*'))
        {

            exp.insert(exp.begin() + i, '.');
        }
    }
    cout << exp << '\n';
}

int Precedence(char sym)
{
    int priority;
    switch (sym)
    {
    case '|':
        priority = 1;
        break;
    case '.':
        priority = 2;
        break;
    case '*':
        priority = 3;
        break;
    default:
        priority = 0;
    }
    return priority;
}

void RegexExpToPost(const string &exp)
{
    stack<char> st;
    st.push('#'); //占位符，这样就不需要判断栈是否为空

    for (auto ch : exp)
    {
        if (ch == '(')
        {
            st.push(ch);
        }
        else if (ch == ')')
        {
            while (st.top() != '(')
            {
                post += st.top();
                st.pop();
            }
            st.pop();
        }
        else if (ch == '|' || ch == '*' || ch == '.')
        {
            char c1 = st.top();
            while (Precedence(c1) >= Precedence(ch))
            {
                post += c1;
                st.pop();
                c1 = st.top();
            }
            st.push(ch);
        }
        else
        {
            post += ch;
        }
    }
    while (!st.empty() && st.top() != '#')
    {
        post += st.top();
        st.pop();
    }
}

void ThompsonConstruction()
{
    stack<int> states;
    NFAGraph->InsertVertex(0); //起始节点，编号为0
    int left, right, m1, m2;   //i表示顶点编号
    int i = 1;
    for (auto ch : post)
    {
        if (ch == '.')
        { //left -> m1 -> m2 -> right
            right = states.top(), states.pop();
            m2 = states.top(), states.pop();
            m1 = states.top(), states.pop();
            left = states.top(), states.pop();
            NFAGraph->InsertEdge(m1, m2, '~');
            states.push(left);
            states.push(right);
        }
        /*    -> left -> right
		   i                    i + 1
			  -> m1   -> m2
		*/
        else if (ch == '|')
        {
            right = states.top(), states.pop();
            left = states.top(), states.pop();
            m2 = states.top(), states.pop();
            m1 = states.top(), states.pop();
            NFAGraph->InsertVertex(i);
            NFAGraph->InsertVertex(i + 1);
            NFAGraph->InsertEdge(i, m1, '~');
            NFAGraph->InsertEdge(i, left, '~');
            NFAGraph->InsertEdge(m2, i + 1, '~');
            NFAGraph->InsertEdge(right, i + 1, '~');
            states.push(i);
            states.push(i + 1);
            i += 2;
        }
        /*				<--
			 i ->  left -> right  -> i + 1
			          -------->
		*/
        else if (ch == '*')
        {
            right = states.top(), states.pop();
            left = states.top(), states.pop();
            NFAGraph->InsertVertex(i);
            NFAGraph->InsertVertex(i + 1);
            NFAGraph->InsertEdge(i, i + 1, '~');
            NFAGraph->InsertEdge(i, left, '~');
            NFAGraph->InsertEdge(right, i + 1, '~');
            NFAGraph->InsertEdge(right, left, '~');
            states.push(i);
            states.push(i + 1);
            i += 2;
        }
        /*	  ch	
			i -> i + 1
		*/
        else
        {
            NFAGraph->InsertVertex(i);
            NFAGraph->InsertVertex(i + 1);
            NFAGraph->InsertEdge(i, i + 1, ch);
            states.push(i);
            states.push(i + 1);
            i += 2;
        }
    }
    //构建初始节点的Epison指向
    right = states.top(), states.pop();
    left = states.top(), states.pop();
    NFAGraph->InsertEdge(0, left, '~');

    NFAStatesNumber = right + 1; //状态数
}

void GetEdgesCharacter()
{
    for (auto ch : post)
    {
        if (ch != '.' && ch != '|' && ch != '*')
        {
            edges.insert(ch);
        }
    }
}

void SubSetConstruction()
{
    Dtran.resize(NFAStatesNumber);
    set<int> st, temp;
    st.insert(0);

    st = NFAGraph->Closure(st);
    DStates.push_back(st);

    int index = 0;
    while (1)
    {
        st = DStates[index];
        for (auto edge : edges)
        {
            temp = NFAGraph->Closure(NFAGraph->Move(st, edge));
            if (temp.size() > 0)
            {
                auto state = find(DStates.begin(), DStates.end(), temp);
                if (state != DStates.end())
                {
                    Dtran[index][edge] = state - DStates.begin();
                }
                else
                {
                    Dtran[index][edge] = DStates.size();
                    DStates.emplace_back(temp);
                }
            }
        }
        index++;

        //因为后面新增加的状态也需要遍历，所以这个index相当于标记
        if (index == DStates.size())
            break;
    }

    Dtran.resize(index);
    DtranNumber = index;

    //DFA图哪些状态是终结态？
    for (int i = 0; i < DStates.size(); ++i)
    {
        for (auto it : DStates[i])
        {
            if (it == NFAStatesNumber - 1)
            {
                AcceptStates.insert(i);
                break;
            }
        }
    }
}

bool MinimizeDFAStates(set<int> &DelSet)
{
    bool flag = true;
    for (int i = 0; i < DtranNumber - 1; ++i)
    {
        for (int j = i + 1; j < DtranNumber; ++j)
        {
            if (DelSet.count(i) || DelSet.count(j))
                continue;
            if (!AcceptStates.count(i) && AcceptStates.count(j))
                continue;
            if (AcceptStates.count(i) && !AcceptStates.count(j))
                continue;
            if (Dtran[i].size() != Dtran[j].size())
                continue;
            bool check = true;
            //check if(i == j) in all change
            for (auto it : Dtran[i])
            {
                if (!Dtran[j].count(it.first) || Dtran[j][it.first] != it.second)
                {
                    check = false;
                    break;
                }
            }
            if (check)
            {
                flag = false;
                for (int k = 0; k < DtranNumber; ++i)
                {
                    if (DelSet.count(k))
                        continue;
                    //记得加引用
                    for (auto &it : Dtran[k])
                    {
                        if (it.second == j)
                        {
                            it.second = i;
                        }
                    }
                }
                AcceptStates.erase(j);
                DelSet.insert(j);
            }
        }
    }
    return !flag;
}

void MinDfa()
{
    set<int> DelSet;
    bool flag = true;
    flag = MinimizeDFAStates(DelSet);
    while (flag)
    {
        flag = MinimizeDFAStates(DelSet);
    }
    // reserve:
    // 0 1 x 3 x 5
    // 0 1   2   3
    int cnt = 0;
    map<int, int> reserve;
    for (int i = 0; i < Dtran.size(); ++i)
    {
        if (DelSet.find(i) == DelSet.end())
            reserve[i] = cnt++;
    }
    vector<map<char, int>> Dt;
    //reserve: 0 1 3 5
    for (auto it1 : reserve)
    {
        map<char, int> temp;
        for (auto it2 : Dtran[it1.first])
        {
            temp[it2.first] = reserve[it2.second];
        }
        Dt.push_back(temp); //Dt: 0 1 2 3
    }
    Dtran = std::move(Dt);
    DtranNumber = Dtran.size();

    set<int> temp_acc;
    for (const auto &accstate : AcceptStates)
    {
        int cnt = accstate;
        for (const auto &del : DelSet)
        {
            if (del < cnt)
                cnt--;
        }
        temp_acc.insert(cnt);
    }
    AcceptStates = move(temp_acc);
}

#define lpb(x) lines.push_back(x)

void PrintProgram()
{
    vector<string> lines;
    lpb("#include <bits/stdc++.h>");
    lpb("using namespace std;");

    string s = "char edge[] = {";
    for(auto edge: edges)
    {
        s += "'" + edge;
        s += "', ";
    }
    s += "};";
    lpb(s);

    s = "set<int> AcceptStates{";
    for (auto accstate : AcceptStates)
    {
        s += char(accstate + '0') + ", ";
    }
    s += "}";
    lpb(s);

    s = "vector<vector<int>> DFATransfer{";
    for (int i = 0; i < DtranNumber; ++i)
    {
        s += "{";
        for (auto edge : edges)
        {
            if(Dtran[i].count(edge)){
                s += char(Dtran[i][edge] + '0') + ',';
            }else{
                s += "-1,";
            }
        }
        s += "},";
    }
    s += "},";
    lpb(s);
    //...
    //生成getpos函数
    //生成main函数
    //switch case
    for(auto edge: edges)
    {
        s = "case '" + edge;
        s += "':";
        lpb(s);
        lpb("pos = getpos(str[j]);");
        lpb("if (pos != -1 && state != -1){");
        lpb("state = DFATransfer[state][pos];}");
        lpb("else{cerr<<\"No\"<<endl;return -1;}");
        lpb("break;");
    }
    lpb("default:cout<<\"No\"; return 0;");
    lpb("}"); //switch -- while
    //...
    lpb("if(AcceptStates.count(state)){");
    lpb("cout<<\"yes\"<<endl'}");
    lpb("else{cout<<\"No\"<<endl;}");
    lpb("return 0;}");
}