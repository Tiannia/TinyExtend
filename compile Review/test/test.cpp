#include <string>
#include <iostream>
using namespace std;

enum TypeA{
    apple,
    banana,
    pie
};

int main()
{
    TypeA a;
    a = TypeA(0);
    cout << a + 1 << endl;
    return 0;
}