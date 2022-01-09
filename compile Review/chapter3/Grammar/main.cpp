#include "ll1node.h"
#define MAXNODE 1001

vector<LL1Node> grammers;
set<char> finalword, nonfinalword;
vector<map<char, string>> table;
map<char, set<char>> first;
map<char, set<char>> follow;
map<string, set<char>> select;

int visit[MAXNODE];

//注意一个前提：
//不存在     A -> ab
//          A -> C
//全部都为   A -> ab | C

set<char> getSingleFirst(char left);

int getNodeFinalWordIndex(char left)
{
    for (int i = 0; i < grammers.size(); ++i)
    {
        if (left == grammers[i].left)
            return i;
    }
    return -1;
}

void initWords()
{
    set<char> wordset;
    nonfinalword.clear();
    finalword.clear();
    for (auto &gram : grammers)
    {
        wordset.insert(gram.left);
    }
    for (auto &gram : grammers)
    {
        for (auto &right : gram.right)
        {
            for (const auto &word : right)
            {
                if (!wordset.count(word))
                    finalword.insert(word);
            }
        }
    }
    nonfinalword = wordset;
}

//文法化简
void simplify()
{
    //不可到达（左边推右边）
    //vn : nonfinal
    //vt : final
    set<char> vt;
    set<char> vn;
    vn.insert(grammers[0].left);
    bool flag = true;
    while (flag)
    {
        flag = false;
        for (auto gram : grammers)
        {
            if (!vn.count(gram.left))
                continue;
            for (const auto &right : gram.right)
            {
                for (const auto &word : right)
                {
                    if (finalword.count(word) && !vt.count(word)) //终结符
                    {
                        vt.insert(word);
                        flag = true;
                    }
                    else if (nonfinalword.count(word) && !vn.count(word)) //非终结符
                    {
                        vn.insert(word);
                        flag = true;
                    }
                }
            }
        }
    }
    //左符号有，右边一定全部有
    vector<LL1Node> temp0_grammers;
    for (int i = 0; i < grammers.size(); ++i)
    {
        if (vn.count(grammers[i].left))
        {
            temp0_grammers.push_back(grammers[i]);
        }
    }
    grammers = temp0_grammers;
    nonfinalword = vn;
    finalword = vt;

    //不可终止（右边推左边）
    set<char> vn1;
    bool flag = true;
    while (flag)
    {
        flag = false;
        for (auto gram : grammers)
        {
            if (vn1.count(gram.left))
                continue;
            for (const auto &right : gram.right) //“|”
            {
                bool tflag = true;
                for (const auto &word : right)
                {
                    //如果是非终结符 且 非该非终结符不在vn1集合中
                    if (!vn1.count(word) && nonfinalword.count(word))
                    {
                        tflag = false;
                        break;
                    }
                }
                if (tflag)
                {
                    vn1.insert(gram.left);
                    flag = true;
                    break;
                }
            }
        }
    }

    vector<LL1Node> temp1_grammers;
    for (int i = 0; i < grammers.size(); ++i)
    {
        if (!vn1.count(grammers[i].left))
            continue;
        LL1Node node(node.left);
        for (const auto &right : grammers[i].right)
        {
            bool temp = true;
            for (const auto &word : right)
            {
                if (!vn1.count(word) && !finalword.count(word))
                {
                    temp = false;
                    break;
                }
            }
            if (temp)
            {
                node.insert(right);
            }
        }
        temp1_grammers.push_back(node);
    }
    grammers = temp1_grammers;
}

vector<LL1Node> dealCommonGrammers(LL1Node node)
{
    //先处理递归的情况：
    // A -> ad | Bc
    // B -> ae
    vector<LL1Node> afterDeal;
    for (int i = 0; i < node.right.size(); ++i)
    {
        string right = node.right[i];
        if (nonfinalword.count(right[0]))
        {
            //let ae replace B
            //应该有个递归的过程
        }
    }
    for (int i = 0; i < node.right.size(); ++i)
    {
        char Ifirst = node.right[i][0];

        string JnoCommonLefts;
        vector<string> noCommonTemp;
        bool hasCommon = false;
        for (int j = i + 1; i < node.right.size(); ++j)
        {
            char Jfirst = node.right[j][0];
            if (Ifirst == Jfirst)
            {
                hasCommon = true;
                JnoCommonLefts.assign(node.right[j].begin() + 1, node.right[j].end());
                if (JnoCommonLefts.size() == 0)
                {
                    JnoCommonLefts.push_back('@');
                }
            }
            else
            {
                noCommonTemp.push_back(node.right[j]);
            }
        }
        if (hasCommon)
        {
            string InoCommonLefts;
            InoCommonLefts.assign(
                node.right[i].begin() + 1,
                node.right[i].end());
            if (InoCommonLefts.size() == 0)
            {
                InoCommonLefts.push_back('@');
            }
            //while:
            char newnodename = rand() % 27 + 'A';
            LL1Node newnode(newnodename);
            newnode.insert(InoCommonLefts);
            newnode.insert(JnoCommonLefts);

            LL1Node orinode(node.left);
            string oriname = "";
            oriname.push_back(Ifirst);
            oriname.push_back(newnodename);

            orinode.insert(oriname);
            orinode.insert(noCommonTemp.begin(), noCommonTemp.end());

            //把新生成的两个规则放入afterDeal中
            afterDeal.push_back(newnode);
            afterDeal.push_back(orinode);
            break;
        }
    }
    return afterDeal;
}

//间接 再 直接
void dealLeftCommonGrammers()
{
    char beside = grammers[0].left;
    for (int i = 0; i < grammers.size(); ++i)
    {
        vector<LL1Node> afterDeal = dealCommonGrammers(grammers[i]);
        if (afterDeal.size())
        {
            grammers.erase(grammers.begin() + i);
            grammers.insert(grammers.end(), afterDeal.begin(), afterDeal.end());
        }
    }
    int after = getNodeFinalWordIndex(beside);
    assert(after >= 0);

    swap(grammers[0], grammers[after - 1]);
    initWords();
}

void removeDirectLeftRecursion()
{
    for (int i = 0; i < grammers.size(); ++i)
    {
        char check = grammers[i].left;
        auto rights = grammers[i].right;
        //while
        char newLeft = rand() % 27 + 'A';
        bool flag = true;
        for (int i = 0; i < rights.size(); ++i)
        {
            if (rights[i][0] == check)
            {
                grammers.push_back(LL1Node(newLeft));
                flag = false;
                break;
            }
        }
        if (flag)
            continue;
        vector<string> oriRule;
        vector<string> &newRule = grammers.back().right;
        newRule.push_back("@"); // @ | ...

        for (int i = 0; i < rights.size(); ++i)
        {
            if (rights[i][0] == check) //存在左递归 A
            {
                string temp(rights[i].begin() + 1, rights[i].end());
                temp.push_back(newLeft);
                newRule.push_back(temp);
            }
            else
            {
                string temp(rights.begin(), rights.end());
                temp.push_back(newLeft);
                oriRule.push_back(temp);
            }
        }

        grammers[i].right = oriRule;
    }
}

void dealLeftRecursion()
{
    for (int i = 0; i < grammers.size(); ++i)
    {
        for (int j = 0; j < i; ++j)
        {
            //代入消除
        }
    }

    removeDirectLeftRecursion();
    initWords();
    simplify();
    initWords();
}

void dfsFindFirst(int i)
{
    if (visit[i])
        return;
    visit[i] = 1;
    char left = grammers[i].left;
    for (auto right : grammers[i].right)
    {
        for (int i = 0; i < right.size(); ++i)
        {
            char word = right[i];
            if (finalword.count(word))
            {
                first[left].insert(word);
                break;
            }
            else
            {
                int nxt = getNodeFinalWordIndex(word);
                char nleft = grammers[nxt].left;
                dfsFindFirst(nxt);
                bool flag = true;
                for (auto it : first[nleft])
                {
                    if (it == '@')
                        flag = false;
                    else
                    {
                        first[left].insert(it);
                    }
                }
                if (!flag)
                {
                    if (i == right.size() - 1)
                        first[left].insert('@');
                    //keep going
                }
                else
                {
                    break;
                }
            }
        }
    }
}

void makeFirstSet()
{
    memset(visit, 0, sizeof(visit));
    for (int i = 0; i < grammers.size(); ++i)
    {
        if (!visit[i])
            dfsFindFirst(i);
    }
}

void makeFollowSet()
{
    //step1:
    follow[grammers[0].left].insert('$');

    //连续使用下面的规则，直到每个集合不再增大为止。
    bool KEEPGOING = true;
    while (KEEPGOING)
    {
        KEEPGOING = false;
        for (int i = 0; i < grammers.size(); ++i)
        {
            const char &left = grammers[i].left;
            const auto &rights = grammers[i].right;
            for (auto right : rights)
            {
                bool flag = true;
                for (int j = right.size() - 1; j >= 0; --j)
                {
                    //逆序j
                    //然后对每一个j，我们看它后面那一个符号
                    char word = right[j];
                    // A -> αBβ (put FISRT(β) but not @ in FOLLOW(B))
                    if (nonfinalword.count(word))
                    {
                        if (flag)
                        {
                            //PUT FOLLOW(A) IN FOLLOW(B)
                            set<char> from = follow[left];
                            set<char> &to = follow[word];
                            int before = to.size();
                            for (auto it : from)
                            {
                                to.insert(it);
                            }
                            if (to.size() > before)
                                KEEPGOING = true;
                            if (!first[word].count('@')) //因为是倒序遍历，所以当前符号不包含@的话，后面也就不能执行这个步骤了
                                flag = false;
                        }
                        while (j + 1 < right.size())
                        {
                            char nxtword = right[j + 1]; //我们来看看下一个字符吧~
                            // nxtword is nonfinalword
                            if (nonfinalword.count(nxtword))
                            {
                                set<char> from = first[nxtword];
                                set<char> &to = follow[word];
                                int before = to.size();
                                for (auto it : from)
                                {
                                    if (it != '@')
                                        to.insert(it);
                                }
                                if (to.size() > before)
                                    KEEPGOING = true;
                                if (!from.count('@'))  //有@才能继续放入First(j+1)
                                    break;
                            }
                            // nxtword is finalword
                            else
                            {
                                int before = follow[word].size();
                                follow[word].insert(nxtword);
                                if (follow[word].size() > before)
                                    KEEPGOING = true;
                                break;
                            }
                            j++;
                        }
                    }
                    else
                    {
                        flag = false;
                    }
                }
            }
        }
    }
}

void getSelect()
{
    for (auto gramm : grammers)
    {
        char left = gramm.left;
        vector<string> rights = gramm.right;
        set<char> all;
        for (auto right : rights)
        {
            for (int i = 0; i < right.size(); ++i)
            {
                char word = right[0];
                if (nonfinalword.count(word)) //非终结符
                {
                    for (auto it : first[word])
                    {
                        if (it != '@')
                            all.insert(it);
                    }
                    if (!first[word].count('@'))
                        break;
                    if (i == right.size() - 1)
                    {
                        for (auto it : follow[left])
                        {
                            all.insert(it);
                        }
                    }
                }
                else //终结符
                {
                    if (word == '@')
                    {
                        for (auto it : follow[left])
                        {
                            all.insert(it);
                        }
                    }
                    else
                    {
                        all.insert(word);
                    }
                    break;
                }
            }
            string s;
            s.push_back(left);
            //we use "s->ab" to distinguish different transforation with same left
            select[s + "->" + right] = all;
        }
    }
}

void makeTable()
{
    //vector<map<char, string> table;
    table.resize(grammers.size());
    getSelect();
    // A->Bcd = {a , c , d, $}
    for (int i = 0; i < grammers.size(); ++i)
    {
        char left = grammers[i].left;
        for (auto it : select)
        {
            if (left == it.first[0])
            {
                for (auto letter : it.second)
                {
                    table[i][letter] = it.first;
                }
            }
        }
    }
}

void analysisExp(string text)
{
    stack<char> stk;
    text.push_back('$');
    stk.push('$');
    stk.push(grammers[0].left);
    int idx = 0, num = 1;
    bool error = false;
    string msg;
    while (true)
    {
        msg = "";
        char x = stk.top();
        if (finalword.count(x) || '$' == x)
        {
            if (x == text[idx])
            {
                if ('$' == x)
                    //call success;
                    break;
                else
                {
                    msg = "match!";
                    //printstep;
                    stk.pop();
                }
            }
            else
            {
                //call error;
            }
        }
        else
        {
            int pos = getNodeFinalWordIndex(x);
            if (!table[pos].count(text[idx]))
            {
                //call error;
            }
            string msg = table[pos][text[idx]];
            string temp = msg.substr(3);
            //printstep;
            stk.pop();
            if (temp[0] == '@')
                continue;
            for (int i = temp.size() - 1; i >= 0; --i)
            {
                stk.push(temp[i]);
            }
        }
    }
}
