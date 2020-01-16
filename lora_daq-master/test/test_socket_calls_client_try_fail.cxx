#include "old_Socket_Calls.h"
#include <iostream>
#include <string>
#include <unistd.h>
#include <sstream>

void print_array(unsigned char* arr, int len)
{
  for (int i=0; i<len; i++)
  {
    std::cout << arr[i] << " ";
  }

  std::cout << std::endl;
}

int main()
{
    std::string port_string1="3301";
    std::string m_name="131.174.114.11";
    unsigned char read_data1[40];
    int bytes_sent, bytes_recv;

    for (int i=0;i<1000;i++)
    {
      SOCKET_CALLS sc_client1(m_name,port_string1);
      sc_client1.Open();
      sc_client1.Connect();
      int read_bytes1=0;
      sc_client1.Receive_unsigned_char_buff(read_data1,40,read_bytes1);
      std::cout << "1 : ";
      print_array(read_data1,40);
      sc_client1.Close();
      sleep (5);
    }
}
