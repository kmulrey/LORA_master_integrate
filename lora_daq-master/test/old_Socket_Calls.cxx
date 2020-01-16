
#include "old_Socket_Calls.h"
//C++ libraries.
#include <cstdio>
#include <cstdlib>
#include <cstring> //memset, strcpy, etc.
#include <string> // std::string
#include <stdexcept>
#include <iostream>
#include <cerrno>
#include <sstream> // for int to str conversion in Send().


//OS Libraries
#include <unistd.h> //for close(), read(), etc.
#include <netdb.h> //gai_streror()
#include <arpa/inet.h> // inet_pton(), inet_hton(), etc. network byte order converters are here.

//get sockaddr, IPV4 or IPV6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return & ( ( (struct sockaddr_in*)sa)->sin_addr);
    }
    return & ( ( (struct sockaddr_in6*)sa)->sin6_addr);
}


/*
**********************************************************
METHODS FOR: CLIENTS AND SERVERS
**********************************************************
*/
SOCKET_CALLS::SOCKET_CALLS(const std::string& m_name, const std::string& m_port)
{
    /*
    Constructor for this class. Needs target machine name and port number as strings.

    * input: const char* m_name : name of the host
    * input: const char* m_port : port number on the host
    * action: populate sc_machine_name, sc_port_string member vars
    * action: obtain a "good" value for "sc_addr" var of type addrinfo
    * note: a bunch of sockets might be generated (depends on how many addrinfos getaddrinfo() returns) but all will be closed. actual defining of sockfd happens in Open() method.


    ** Step 0: construct addrCriteria to be passed to the getaddrinfo
    ** Step 1: Use getaddrinfo() to get a linked list
    ** Step 2: Find a valid addr struct from linked list and store it in addr_
    ** Step 3: free up the linked list. crucial. can lead to memory issues.

    */

     //    set values of member vars for machine name and port no.
     sc_machine_name=m_name;
     sc_port_string=m_port;


     //some temporary vars
     struct addrinfo addrCriteria, *addrList, *p ;

     //setup addrCriteria
     std::memset(&addrCriteria, 0, sizeof addrCriteria);
     addrCriteria.ai_family = sc_ip_family;
     addrCriteria.ai_socktype = sc_sock_type;

     // give inputs and obtain &addrList i.e. list of linked structs.
     int temp_val = getaddrinfo(sc_machine_name.c_str(), sc_port_string.c_str(), &addrCriteria, &addrList);

     //check whether getaddrinfo() failed
     if (temp_val!=0) {
         std::string errormsg = "SOCKET_CALLS constructor says: getaddrinfo() failed.";
         errormsg+= std::string(gai_strerror(temp_val));
         throw std::runtime_error(errormsg);
     }

     //loop over addrList and find one that works.
     for (p=addrList; p!=NULL; p=p->ai_next)
     {
        int temp_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        if (temp_sockfd==-1)
        {
            std::string errormsg="SOCKET_CALLS constructor says: socket() not found, will go to next addrinfo in addrList.";
            errormsg+= std::string(std::strerror(errno));
            std::cout<< errormsg << std::endl;
            continue;
        }

        //store a good struct from the list into the member var sc_addr
        sc_addr = *p;

        //close the socket which was successfully made for trial purposes.
        //actual socket will be made in the method Open().
        close(temp_sockfd);

        break;
     }

     //if no good addrinfo was found in addrList, throw an error.
     if (p==NULL){
        std::string errormsg = "SOCKET_CALLS constructor says: failed to find good sock_addr in addrList returned by getaddrinfo()";
        throw std::runtime_error(errormsg) ;
     }

     // free up the memory occupied by linked list of addrinfo structs.
     freeaddrinfo(addrList);
}



void SOCKET_CALLS::Open()
{
    /*
    Generates a socket file descriptor.
    This method must not fail since check was performed in SOCKET_CALLS() constructor.
    */
    sc_init_sockfd = socket(sc_addr.ai_family, sc_addr.ai_socktype, sc_addr.ai_protocol);
    if (sc_init_sockfd==-1)
    {
            std::string errormsg="Open() says: socket() fails. this shouldn't have happened. SOCKET_CALLS() constructor checks for this!";
            errormsg+= std::string(std::strerror(errno));
            throw std::runtime_error(errormsg);
        }
    sc_active_sockfd = sc_init_sockfd;
}


void SOCKET_CALLS::Send(const std::string& data, int& write_bytes)
{
    const char* buffer = data.c_str();
    int sizeofdata = data.size();

    if (sizeofdata> (sc_max_buffer_size - 1)) // because buffer includes the last null \0 element.
    {
        std::string errormsg = "Send() error. sizeof message is >";
        //convert buffer size from int to str. have to jump through hoops to make it happen in cpp.
        std::stringstream ss;
        ss << sc_max_buffer_size;
        errormsg+= std::string(ss.str());
        errormsg+= std::string(std::strerror(errno));
        throw std::runtime_error(errormsg);
    }

    //using active_sockfd , so that for clients it will stay as init.
    //whereas for servers it will be listening_sockfd
    write_bytes=write(sc_active_sockfd, buffer, sizeofdata);
    std::cout << "write bytes=" << write_bytes << std::endl;
}

int SOCKET_CALLS::Receive_unsigned_char_buff(unsigned char* buffer, size_t sizeofbuffer, int& read_bytes)
{
    //read() populates the buffer char array and read_bytes value.

    //using active_sockfd in case Listen() was called before this call.
    read_bytes=read(sc_active_sockfd, buffer, sizeofbuffer);

	  std::cout<<"read bytes=" << read_bytes << std::endl;

    return errno;
}

void SOCKET_CALLS::Receive(std::string& data, int& read_bytes)
{
    char buffer[sc_max_buffer_size];
    size_t sizeofbuffer = sizeof buffer;

    //using active_sockfd in case Listen() was called before this call.
    read_bytes=read(sc_active_sockfd, buffer, sizeofbuffer);

	std::cout<<"read bytes=" << read_bytes << std::endl;

    buffer[read_bytes]='\0';
    //termination character. tells that string ends here.

	data = std::string(buffer);
}


// Close both initial and accepted sock file descriptors.
void SOCKET_CALLS::Close()
{
    // check if sc_listening_sockfd was opened.
    if (sc_active_sockfd==sc_listening_sockfd) close(sc_listening_sockfd);
	close(sc_init_sockfd);
}


/*
**********************************************************
METHODS FOR: SERVERS
**********************************************************
*/
void SOCKET_CALLS::Bind()
{
    /*
    First Step for a Server after Open().
    you want to listen to init sock fd (active==init so far. not using active here.)
    */
	int on=1 ;
	int temp_val;

	// in case socket was in use before, this will make it free without waiting for it to free up.
	temp_val = setsockopt(sc_init_sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) ) ;
	if (temp_val==-1)
	{
        Close(); // close sockets before failing the code. be nice.
        std::string errormsg="Server says: setsockopt for SOCKET REUSE fails.";
        errormsg+= std::string(std::strerror(errno));
	    throw std::runtime_error(errormsg);
	}

	// bind to the now-free socket
    temp_val = bind(sc_init_sockfd,sc_addr.ai_addr, sc_addr.ai_addrlen);
	if (temp_val==-1)
	{
        Close(); // close sockets before failing the code. be nice.
        std::string errormsg="Server says: Bind() Error.";
        errormsg+= std::string(std::strerror(errno));
	    throw std::runtime_error(errormsg);
	}
}

// void SOCKET_CALLS::Bind_REUSEPORT()
// {
//     /*
//     First Step for a Server after Open().
//     you want to listen to init sock fd (active==init so far. not using active here.)
//     */
// 	int on=1 ;
// 	int temp_val;
//
// 	// in case socket was in use before, this will make it free without waiting for it to free up.
// 	temp_val = setsockopt(sc_init_sockfd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on) ) ;
// 	if (temp_val==-1)
// 	{
//         Close(); // close sockets before failing the code. be nice.
//         std::string errormsg="Server says: setsockopt for SOCKET REUSE fails.";
//         errormsg+= std::string(std::strerror(errno));
// 	    throw std::runtime_error(errormsg);
// 	}
//
// 	// bind to the now-free socket
//     temp_val = bind(sc_init_sockfd,sc_addr.ai_addr, sc_addr.ai_addrlen);
// 	if (temp_val==-1)
// 	{
//         Close(); // close sockets before failing the code. be nice.
//         std::string errormsg="Server says: Bind() Error.";
//         errormsg+= std::string(std::strerror(errno));
// 	    throw std::runtime_error(errormsg);
// 	}
// }

void SOCKET_CALLS::Listen()
{
    /*
    Second Step for a Server after Open().
    you want to listen to init sock fd (active==init so far. not using active.)
    */
    int BACKLOG= 1;//how many pending connections queue will hold

	if(listen(sc_init_sockfd,BACKLOG)==-1)
	{
        Close(); // close sockets before failing the code. be nice.
        std::string errormsg="Server says: Listen() Error.";
        errormsg+= std::string(std::strerror(errno));
	    throw std::runtime_error(errormsg);
	}
	std::cout << "Server says: Listen() Successful." << std::endl;
}


void SOCKET_CALLS::Accept()
{
	struct sockaddr_storage their_addr ;
	socklen_t addr_size ;
	addr_size=sizeof their_addr ;

	sc_listening_sockfd=accept(sc_init_sockfd,(struct sockaddr*) &their_addr,&addr_size);

	if(sc_listening_sockfd==-1)
	{
        Close(); // close sockets before failing the code. be nice.
        std::string errormsg="Server says: Accept() Error.";
        errormsg+= std::string(std::strerror(errno));
	    throw std::runtime_error(errormsg);
	}

    //now onwards send / recv have to happen on this listening_sockfd.
    sc_active_sockfd= sc_listening_sockfd;
	char s[INET6_ADDRSTRLEN];
	inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s , sizeof s);
    //network to presentation byte ordering transformation
    //
}

bool SOCKET_CALLS::Get_Accept_Status()
{
    //returns true if the Accept() method was a success and the listening_sockfd has been found.
    return (sc_active_sockfd==sc_listening_sockfd && sc_active_sockfd!=0);
}


/*
**********************************************************
METHODS FOR: CLIENTS
**********************************************************
*/
void SOCKET_CALLS::Connect()
{
    /*
    Method For Clients
    Connects to the sc_init_sockfd made by method OPEN()
    For clients, active == init always. using init.
    */
    int temp_val = connect(sc_init_sockfd, sc_addr.ai_addr, sc_addr.ai_addrlen);
    if (temp_val==-1)
    {
        Close(); // close sockets before failing the code. be nice.
        std::string errormsg="Client says: Failed to Connect().";
        errormsg+= std::string(std::strerror(errno));
        std::cout << errormsg <<std::endl;
        return;
	      //throw std::runtime_error(errormsg);
    }

    std::cout<< "Client says: Successful Connect() to machine: " ;
    std::cout << sc_machine_name << " port: " << sc_port_string << std::endl;
}
