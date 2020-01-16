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
    //add useless comment
    std::string port_string="4301";
    std::string m_name="coma01.science.ru.nl";
    std::string send_data="Client says Hello!";
    std::string read_data;
    int bytes_sent, bytes_recv;

    std::cout << "code started..." << std::endl;
    SOCKET_CALLS sc_client1(m_name,port_string);
    sc_client1.Open();
    std::cout << "socket open..." << std::endl;
    sc_client1.Connect();
    std::cout << send_data << " :Sent." << std::endl;
    sc_client1.Send(send_data, bytes_sent);
    sc_client1.Receive(read_data,bytes_recv);
    std::cout << read_data << " :Recvd." <<std::endl;
    sc_client1.Close();
}
