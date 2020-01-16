#include <boost/circular_buffer.hpp>
#include <iostream>

void char_pushback(boost::circular_buffer<unsigned char> &buf, unsigned char chars[], size_t sizeofchars)
{
// push values of array chars one by one to the buf.
    for (int i=0; i< sizeofchars; i++)
    {
        //printf("pushing back %X into the buffer\n", chars[i]);
        buf.push_back(chars[i]);
    }
}

void print_buffer(const boost::circular_buffer<unsigned char> &buf){

    if (!buf.is_linearized()){
        throw std::runtime_error("Buffer needs to be linearized.");
    }

    //print contents of a circular buffer.
    std::cout << "Printing buffer contents..." << std::endl;
    std::cout << "{";
    for (int i=0; i< buf.size(); i++)
    {
        printf("%X ,",buf[i]);
    }
    std::cout << "} " << std::endl; 
}

void extract_first_msg_in_buf(boost::circular_buffer<unsigned char> &buf, std::vector<unsigned char> &msg){
    // copies msg char array between 0x99 (begin) and 0x66 (end)
    // use this instead of using find_end, find_begin
    // loop over buffer elements only occurs once.
    if (!buf.is_linearized()){
        throw std::runtime_error("Buffer needs to be linearized.");
    }
        
    bool copy=false;
    int start = -1;
    int stop = -1;
    for (int i=0; i< buf.size(); i++){
        switch(buf[i])
        { 
        case 0x99:
        {
            if (copy)
            {
                std::cout << "Found second header while parsing buf. ";
                std::cout << "Contents before second header are erased.\n";
                msg.clear();
                msg.push_back(buf[i]);
                break;
            }
            else
            {
                copy = true;
                msg.push_back(buf[i]);
                start = i;
                break;
            }

        }
        case 0x66:
        {
            msg.push_back(buf[i]);
            copy = false;
            stop = i;
            break;
        }
        default:
            if (copy) 
            {
                msg.push_back(buf[i]);
                break;
            }
        }
        if (stop!=-1 && !copy) break; // exit if stop has been found.
    }
    
    if (stop==-1 || start==-1) // i.e. begin/end was not found:
    {
        msg.clear(); //erase extracted msg.
        //do nothing to the buffer.
    }
    else // otherwise proceed with removing the msg from buffer.
    {
        buf.erase(buf.begin()+start, buf.begin()+stop+1); //erase works on [first, last).
    }
}


int main()
{  
  //int Event_Bytes = 12023; // abt 11 mb is maximum packet size
  boost::circular_buffer<unsigned char> Circ_buf{20480}; // 20 mb circular buffer to read in.
  Circ_buf.linearize();

  std::cout<< "Buffer capacity: " << Circ_buf.capacity() << std::endl;
  std::cout<< "Buffer size now: " << Circ_buf.size() << std::endl << std::endl; 
//----------------------------------------------------------------  
  unsigned char in_packet1[3];
  int sizeofinpacket1 = sizeof in_packet1; // casting size_t as int. 
  std::cout << "Size of incoming packet1: "<< sizeofinpacket1  << std::endl;
  in_packet1[0]= 0x99; // Begin char
  for (int i=1; i< sizeofinpacket1; i++) in_packet1[i] = 0x11;

  std::cout << "Adding the incoming packet1 to the buffer:" << std::endl; 
  char_pushback(Circ_buf, in_packet1, sizeofinpacket1);
  std::cout << "Size of buffer now: " << Circ_buf.size() << std::endl;
  print_buffer(Circ_buf);

  std::cout << "Extracting msg out of the buffer:" << std::endl;
  std::vector <unsigned char> my_msg1;
  extract_first_msg_in_buf(Circ_buf, my_msg1);
 
  std::cout << "msg extracted:" ;
  for (int i=0; i< my_msg1.size(); i++) printf("%X ",my_msg1[i]); 
  std::cout<< ". \nBuffer size now: " << Circ_buf.size() << std::endl;
//----------------------------------------------------------------
  unsigned char in_packet2[4];
  int sizeofinpacket2 = sizeof in_packet2; // casting size_t as int. 
  std::cout << "\nSize of incoming packet2: " << sizeofinpacket2  << std::endl;
  in_packet2[0]= 0x99; // Begin char
  for (int i=1; i< sizeofinpacket2-1; i++) in_packet2[i] = 0x22;
  in_packet2[sizeofinpacket2-1]= 0x66; // End char
  
  std::cout << "Adding the incoming packet2 to the buffer:" << std::endl; 
  char_pushback(Circ_buf, in_packet2, sizeofinpacket2);
  std::cout << "Buffer size now: " << Circ_buf.size() << std::endl;
  print_buffer(Circ_buf);

  std::cout << "Extracting msg out of the buffer:" << std::endl;
  std::vector <unsigned char> my_msg2;
  extract_first_msg_in_buf(Circ_buf, my_msg2);
 
  std::cout << "msg extracted:" ;
  for (int i=0; i< my_msg2.size(); i++) printf("%X ",my_msg2[i]); 
  std::cout<< ". \nBuffer size now: " << Circ_buf.size() << std::endl;
//----------------------------------------------------------------  
  unsigned char in_packet3[4];
  int sizeofinpacket3 = sizeof in_packet3; // casting size_t as int. 
  std::cout << "\nSize of incoming packet3: "<< sizeofinpacket3  << std::endl;
  for (int i=0; i< sizeofinpacket3-1; i++) in_packet3[i] = 0x33;
  in_packet2[sizeofinpacket3-1]= 0x66; // End char  
  
  std::cout << "Adding the incoming packet3 to the buffer:" << std::endl; 
  char_pushback(Circ_buf, in_packet3, sizeofinpacket3);
  std::cout << "Size of buffer now: " << Circ_buf.size() << std::endl;
  print_buffer(Circ_buf);

  std::cout << "Extracting msg out of the buffer:" << std::endl;
  std::vector <unsigned char> my_msg3;
  extract_first_msg_in_buf(Circ_buf, my_msg3);
 
  std::cout << "msg extracted:";
  for (int i=0; i< my_msg3.size(); i++) printf("%X ",my_msg3[i]); 
  std::cout<< ". \nBuffer size now: " << Circ_buf.size() << std::endl;
  
}

//  std::cout << "Copying msg out of the buffer:" << std::endl;
//  std::vector <unsigned char> my_msg = copy_first_msg_in_buf(Circ_buf);



  // clear buffer after msg has been copied. and push the in_packet back in_packet
  
//  if (my_msg.size()!=0) {
//      std::cout << "Clearing the buffer since the following msg was extracted out:" << std::endl;
//       for (int i=0; i< my_msg.size(); i++) printf("%X ",my_msg[i]);
//       std::cout << std::endl;
//       Circ_buf.clear();
//   }

// 
// int find_begin(const boost::circular_buffer<unsigned char> &buf){
//     if (!buf.is_linearized()){
//         throw std::runtime_error("Buffer needs to be linearized.");
//     }
// 
//     if (buf.size()==0) {
//         //buffer is empty.
//         return -1;
//     }
//     
//     for (int i=0; i< buf.size(); i++){
//         if (buf[i]==0x99) return i;
//     }
// 
//     return -1; // begin char 0x99 not found.
// }
// 
// 
// int find_end(const boost::circular_buffer<unsigned char> &buf){
//     if (!buf.is_linearized()){
//         throw std::runtime_error("Buffer needs to be linearized.");
//     }
// 
//     if (buf.size()==0) {
//         //buffer is empty.
//         return -1;
//     }
//     
//     for (int i=0; i< buf.size(); i++){
//         if (buf[i]==0x66) return i;
//     }
// 
//     return -1; // begin char 0x99 not found.
// }
// 
// 
// std::vector <unsigned char> copy_first_msg_in_buf(const boost::circular_buffer<unsigned char> &buf){
//     // copies msg char array between 0x99 (begin) and 0x66 (end)
//     // use this instead of using find_end, find_begin
//     // loop over buffer elements only occurs once.
//     if (!buf.is_linearized()){
//         throw std::runtime_error("Buffer needs to be linearized.");
//     }
// 
//     std::vector <unsigned char> msg;
//     
//     if (buf.size()==0) {
//         //buffer is empty. return empty vector.
//         return msg;
//     }
//     
//     bool copy=false;
//     for (int i=0; i< buf.size(); i++){
//         if (buf[i]==0x99){
//             copy = true;
//             msg.push_back(buf[i]);
//         }
//         else if(buf[i]==0x66){
//             msg.push_back(buf[i]);
//             copy = false;
//             break;
//         }
//         else if (copy) msg.push_back(buf[i]);
//     }
// 
//     return msg; // begin char 0x99 not found.
// }
// 
 