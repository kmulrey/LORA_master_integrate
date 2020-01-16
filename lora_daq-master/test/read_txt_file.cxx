#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <stdexcept>

int main() {
  std::string fname="test_file.txt";
  std::string line;
  std::ifstream infile;

  infile.open(fname);

  if (infile.fail())
  {
    throw std::runtime_error(fname+" : file open failed.");
  }

  while (std::getline(infile, line))
  {
    if (line.at(0)=='#') continue;
    std::cout <<" new line" << std::endl;
    std::stringstream ss(line);
    std::string c1, c2, c3, c4, c5;
    ss >> c1 >> c2 >> c3 >> c4 >> c5 ;
    std::cout << "c1" << c1 << std::endl;
    std::cout << "c3" << c3 << std::endl;
    std::cout << "c4" << c4 << std::endl;
  }

  infile.close();

  return 0;
}
