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
    std::string port_string1="4301";
    std::string port_string2="4302";
    std::string port_string3="4303";
    std::string port_string4="4304";

    std::string m_name="131.174.114.11";
    std::string send_data="Client says Hello!";
    //std::string recv_data="";
    unsigned char read_data1[40];
    unsigned char read_data2[40];
    unsigned char read_data3[40];
    unsigned char read_data4[40];
    int bytes_sent, bytes_recv;

    SOCKET_CALLS sc_client1(m_name,port_string1);
    sc_client1.Open();
    sc_client1.Connect();
    //sc_client1.Send(send_data, bytes_sent);
    sleep(2);

    SOCKET_CALLS sc_client1s(m_name,port_string1);
    sc_client1s.Open();
    sc_client1s.Connect();
    //sc_client1s.Send(send_data, bytes_sent);
    sleep(2);

    SOCKET_CALLS sc_client2(m_name,port_string2);
    sc_client2.Open();
    sc_client2.Connect();
    //sc_client2.Send(send_data, bytes_sent);
    sleep(2);

    SOCKET_CALLS sc_client2s(m_name,port_string2);
    sc_client2s.Open();
    sc_client2s.Connect();
    //sc_client2s.Send(send_data, bytes_sent);
    sleep(2);

    SOCKET_CALLS sc_client3(m_name,port_string3);
    sc_client3.Open();
    sc_client3.Connect();
    //sc_client3.Send(send_data, bytes_sent);
    sleep(2);

    SOCKET_CALLS sc_client3s(m_name,port_string3);
    sc_client3s.Open();
    sc_client3s.Connect();
    //sc_client3s.Send(send_data, bytes_sent);
    sleep(2);

    SOCKET_CALLS sc_client4(m_name,port_string4);
    sc_client4.Open();
    sc_client4.Connect();
    //sc_client4.Send(send_data, bytes_sent);
    sleep(2);

    SOCKET_CALLS sc_client4s(m_name,port_string4);
    sc_client4s.Open();
    sc_client4s.Connect();
    //sc_client4s.Send(send_data, bytes_sent);
    sleep(10);

    int read_bytes1=0;
    sc_client1.Receive_unsigned_char_buff(read_data1,40,read_bytes1);
    std::cout << "1 : ";
    print_array(read_data1,40);

    int read_bytes2=0;
    sc_client2.Receive_unsigned_char_buff(read_data2,40,read_bytes2);
    std::cout << "2 : ";
    print_array(read_data2,40);

    int read_bytes3=0;
    sc_client3.Receive_unsigned_char_buff(read_data3,40,read_bytes3);
    std::cout << "3 : ";
    print_array(read_data3,40);

    int read_bytes4=0;
    sc_client4.Receive_unsigned_char_buff(read_data4,40,read_bytes4);
    std::cout << "4 : ";
    print_array(read_data4,40);

    sleep(300);

    //for (int i=0;i<=200;i++)
    //{
    //  sleep(45);
      // send_data = "Client sends msg no: " + std::to_string(i);
      // std::cout << "Attempting to send: " << send_data << std::endl;
      // sc_client.Send(send_data, bytes_sent);
    //}
    //sc_client.Receive(recv_data, bytes_recv);
    //std::cout<< "Client Received:" << recv_data << std::endl;
    sc_client1.Close();
    sc_client1s.Close();
    sc_client2.Close();
    sc_client3.Close();
    sc_client4.Close();

    sc_client2s.Close();
    sc_client3s.Close();
    sc_client4s.Close();
}
