#include <iostream>
#include <vector>

struct a
{
  int b;
}

int int main()
{
  std::vector<a> my_a;
  std::cout << &my_a;
  return 0;
}
