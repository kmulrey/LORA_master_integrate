#include "old_Socket_Calls.h"
#include <string>
#include <iostream>

int main()
{
    std::string port_string="4301";
    std::string m_name="coma01.science.ru.nl";//coma: "131.174.114.11";
    std::string send_data="Server says Hello!";
    std::string recv_data="";
    int bytes_sent, bytes_recv;

    SOCKET_CALLS sc_server(m_name,port_string);

    sc_server.Open();
    sc_server.Bind();//_REUSEPORT();
    sc_server.Listen();
    std::cout<< "Test Server: waiting for connections..." << std::endl;
    while(1){
        sc_server.Accept();
        if (sc_server.Get_Accept_Status()) break;
    }
    for (int i=0;i<=200;i++)
    {
      std::cout<< "Server Attempting To Receive...." << i << std::endl;
      sc_server.Receive(recv_data, bytes_recv);
      std::cout<<"Server Received:"<< recv_data << std::endl;
    }
    sc_server.Send(send_data, bytes_sent);
    sc_server.Close();
}
