#include <iostream>
#include <memory>
using namespace std;

int main()
{
  int *a = new int(2);
  shared_ptr<int> sp1(a);
  cout << sp1.use_count() << endl;
  shared_ptr<int> sp2 = sp1;
  cout << sp2.use_count() << endl;
  return 0;
}
