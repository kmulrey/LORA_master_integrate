#include <boost/circular_buffer.hpp>
#include <iostream>

int main()
{
  typedef boost::circular_buffer<int> circular_buffer;
  circular_buffer cb{10};

  cb.push_back(0);
  cb.push_back(1);
  cb.push_back(2);
  cb.push_back(3);
  cb.push_back(4);
  cb.push_back(5);
  cb.push_back(6);
  cb.push_back(7);
  cb.push_back(8);
  cb.push_back(9);
  cb.push_back(10);
  cb.push_back(11);
  cb.push_back(12);
  cb.push_back(13);


  std::cout << std::boolalpha << cb.is_linearized() << '\n';

  circular_buffer::array_range ar1, ar2;

  ar1 = cb.array_one(); ar2 = cb.array_two();
  std::cout << ar1.second << ";" << ar2.second << '\n';

  circular_buffer::iterator itstart , itend;
  for (auto it2=cb.begin();it2!=cb.end();++it2)
  {
      std::cout << *it2 << ", ";
      if (*it2==5) itstart = it2;
      if (*it2==10) itend = ++it2;
  }
  std::cout << std::endl;

  cb.erase(itstart,itend);

  for (auto it2=cb.begin();it2!=cb.end();++it2)
  {
      std::cout << *it2 << ", ";
  }
  std::cout << std::endl;

  ar1 = cb.array_one(); ar2 = cb.array_two();
  std::cout << ar1.second << ";" << ar2.second << '\n';

}
