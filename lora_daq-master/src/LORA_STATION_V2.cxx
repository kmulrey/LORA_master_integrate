#include "LORA_STATION_V2.h"
#include "Structs.h"
#include "functions_library.h"
#include "Buffer.h"
#include <string>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory> //unique_ptr
#include <algorithm>
#include <cerrno> //strerror
#include <cstring> //strerror
#include <ctime> //struct tm
#include <utility> //std::pair
#include <numeric>

void LORA_STATION_V2::Init(const STATION_INFO& sta_info, const std::string& server_ip)
{
    
  if (sta_info.type!="clientv2")
  {
    std::stringstream ss;
    ss << "Found STATION_INFO.type: "<< sta_info.type;
    ss << ". LORA_STATION_V2 class applied to a station of type other than clientv2.";
    throw std::runtime_error(ss.str());
  }

  most_recent_msg_rcvd_time[0]=Get_Current_Time_chrono();
  most_recent_msg_rcvd_time[1]=Get_Current_Time_chrono();

  port[0] = sta_info.port_1;
  port[1] = sta_info.port_2;
    std::cout<<"port numbers: "<<port[0]<<"  "<<port[1]<<"\n";

  name = sta_info.name;
  station_no = sta_info.no;
  type = sta_info.type;
    //std::cout<<"station number: "<<station_no<<"\n";
    //std::cout<<"station name: "<<name<<"\n";
    //std::cout<<"station type: "<<sta_info.type<<"\n";

  //HISPARC_Serial[0] = sta_info.HISPARC_Serial_m;
  //HISPARC_Serial[1] = sta_info.HISPARC_Serial_s;

  host_ip = server_ip;
  std::cout<<"server_ip: "<<server_ip<<"\n";

  //load init control param to the container which will be sent to client
  //via the socket. Build_Hisparc_Messages converts unsigned short array
  //to unsigned char array , also doing transformations required to make
  //message readable by Hisparc Digitizers.
  Build_V2_Det_Messages(sta_info.init_control_params_ch[0],Control_Messages[0],0);//katie
  Build_V2_Det_Messages(sta_info.init_control_params_ch[1],Control_Messages[1],1);//katie
  Build_V2_Det_Messages(sta_info.init_control_params_ch[2],Control_Messages[2],2);//katie
  Build_V2_Det_Messages(sta_info.init_control_params_ch[3],Control_Messages[3],3);//katie
  Build_V2_Stn_Messages(sta_info.init_control_params_stn, Control_Mode_Messages);//katie


  //mVolts2ADC is 2

  current_threshold_ADC[0]= (unsigned short) (sta_info.init_control_params_ch[0][8]);
  current_threshold_ADC[1]= (unsigned short) (sta_info.init_control_params_ch[1][8]);
  current_threshold_ADC[2]= (unsigned short) (sta_info.init_control_params_ch[2][8]);
  current_threshold_ADC[3]= (unsigned short) (sta_info.init_control_params_ch[3][8]);

  //socket buffer: holds all incoming msgs.
  // it is frequently checked for a complete msg and msg is moved
  // from here to corresponding spool
  unsigned int size_buf_sock= 10*event_msg_size + 10*onesec_msg_size + 1*cp_msg_size ; //katie: need to change sizes
  // buffer size = f(how fast you empty it, whats the event rate.)

  for (int i=0;i<2;i++)
  {
    std::unique_ptr<BUFFER> temp_ptr1(new BUFFER());
    buf_socket[i] = std::move(temp_ptr1);
    buf_socket[i]->Init(size_buf_sock);
  }

  for (int i=0;i<4;++i)
  {
    hundred_means[i].set_capacity(1000);
    hundred_sigmas[i].set_capacity(1000);
    hundred_thresh_crossing_timestamps[i].set_capacity(1000);
    hundred_timestamps[i].set_capacity(1000);
  }
  hundred_station_event_timestamps.set_capacity(1000);

  idbit_and_msgsize.push_back(std::make_pair(onesec_msg_bit,onesec_msg_size)); //one sec msg
  idbit_and_msgsize.push_back(std::make_pair(event_msg_bit,event_msg_size));//event msg
  idbit_and_msgsize.push_back(std::make_pair(cp_msg_bit,cp_msg_size));//control parameters
    
}

void LORA_STATION_V2::Open()
{
  std::stringstream command, command2;
  //https://serverfault.com/questions/104668/create-screen-and-run-command-without-attaching
  //create a detached screen session:
    
  command2 << "screen -dmS LORA" << station_no;
  system(command2.str().c_str());

  //run the command on the detached screen session:
    
	command << "screen -S LORA" << station_no
          << " -X stuff 'sh ../daq_managers/start_stations_v2.sh " << station_no << " \n'";

	std::cout << "Now starting LORA" << station_no << std::endl;
	system(command.str().c_str());
  
  for (int i=0;i<2;i++) //loop for master, slave. //katie-> loop over 1, only one socket
  {
    bool set_fd_to_nonblock = true;
    // so that we can read repeatedly from it, waiting for EWOULDBLOCK to stop reading.
    //https://eklitzke.org/blocking-io-nonblocking-io-and-epoll
    std::unique_ptr<SOCKET_CALLS> temp_ptr(new SOCKET_CALLS(host_ip, port[i],
                                              set_fd_to_nonblock));
    std::cout<<"host_ip: "<<host_ip<<"  port: "<<port[i]<<"\n";
    socket[i] = std::move(temp_ptr);
    socket[i]->Open();
    socket[i]->Bind();


    socket[i]->Listen();
    //socket[i]->Accept(); //katie: this accept needs to be here
    // // a socket is defined by {SRC-IP, SRC-PORT, DEST-IP, DEST-PORT, PROTOCOL}
    // // src is client(lasa-client) and dest is host (lora main pc that hosts this code.)
    // // as soon as connection is accepted by a socket, a diff sockfd is assigned.
    // // the orig socket connection can Accept() again and create a second sockfd
    // // for this second sockfd, its as good as a second port on DEST side because
    // // on the client side local port is different. i.e. SRC-PORT is diff
    // // this happens automatically because when Connect() is called a client
    // // a local ip and port is assigned to the socket internally.
    //socket[i]->Accept_Spare();
    //FIXIT: accept spare - how to do it under new framework when we do
    //select() on listening sockfd and then accept it. check. relay server doesn't
    //give spare socket as of now. so come to it later.
  }
 // std::cout<<"exiting V2 open\n"; //katie
    

}


void LORA_STATION_V2::Send_Control_Params()//katie
{
    unsigned char Finish_Message[3];
    Finish_Message[0]=((unsigned short)0x99);
    Finish_Message[1]=((unsigned short)0x25);
    Finish_Message[2]=((unsigned short)0x66);
    
  for (int i=1;i<2;i++) //katie: only use send socket
  {
    auto lenofbuffer = sizeof(Control_Messages[0])/sizeof(Control_Messages[0][0]);
    auto lenofbufferMode = sizeof(Control_Mode_Messages);
    auto lenofbufferFinish = sizeof(Finish_Message);

    int bytes_sent = 0;
    bool use_spare_socket=false;
    socket[i]->Send(Control_Messages[0],lenofbuffer,bytes_sent,use_spare_socket);
    usleep(50000);
    socket[i]->Send(Control_Messages[1],lenofbuffer,bytes_sent,use_spare_socket);
    usleep(50000);
    socket[i]->Send(Control_Messages[2],lenofbuffer,bytes_sent,use_spare_socket);
    usleep(50000);
    socket[i]->Send(Control_Messages[3],lenofbuffer,bytes_sent,use_spare_socket);
    usleep(50000);
    socket[i]->Send(Control_Mode_Messages,lenofbufferMode,bytes_sent,use_spare_socket);
    usleep(50000);
    socket[i]->Send(Finish_Message,lenofbufferFinish,bytes_sent,use_spare_socket); // let digitizer know params are done
    std::cout << "Control Params bytes sent: " << bytes_sent << std::endl;
  }
 
}

void LORA_STATION_V2::Send_Electronics_Calib_Msg()
{
    //katie: not needed V2

}

void LORA_STATION_V2::Receive_Electronics_Calib_Msg(int& detectors_calibrated,
                                                    const std::string& ecalib_fname,
                                                    const STATION_INFO& sta_info)
{
    //katie: not needed V2
   
  return;
}

void LORA_STATION_V2::Add_readfds_To_List(fd_set& active_read_fds, int& max_fd_val)
{
  //each station should add using FD_SET to active_stations_fds
  //and replace value in max_fd_val if one of its fds has a higher value.

  for (int i=0;i<2;i++)
  {
    FD_SET(socket[i]->sc_active_sockfd, &active_read_fds);
    max_fd_val = std::max({max_fd_val,
                          socket[i]->sc_active_sockfd});

    //you are not reading from spare socket - so these have been commented out:
    // if (socket[i]->Get_Accept_Spare_Status())
    // {
    //   FD_SET(socket[i]->sc_active_spare_sockfd, &active_read_fds);
    //   max_fd_val = std::max({max_fd_val,
    //                         socket[i]->sc_active_spare_sockfd});
    // }
  }
 
}

int LORA_STATION_V2::Accept(fd_set& fd_list)
{
  
  int n_accepted_connections = 0;
  for (int i=0;i<2;i++) // master, slave. 2 sockets.
  {
    if (!FD_ISSET(socket[i]->sc_active_sockfd, &fd_list)) continue;

    if (!socket[i]->Get_Accept_Status())
    {
      //when its ready for the first time
      //accept the connection. internally socket fd will be
      //replaced by the accepted connection fd.
      socket[i]->Accept();
      ++n_accepted_connections;
      most_recent_msg_rcvd_time[i]=Get_Current_Time_chrono();
    }
    /*
    if (!socket[i]->Get_Accept_Spare_Status())
    {
      socket[i]->Accept_Spare();
      ++n_accepted_connections;
      most_recent_msg_rcvd_time[i]=Get_Current_Time_chrono();
    }
     */
  }
  return n_accepted_connections;
    
    //return 0;
}

void LORA_STATION_V2::Listen(fd_set& fd_list)
{
  
  for (int m_or_s=0; m_or_s<1; m_or_s++) // master, slave. 2 sockets.//katie
  {
    if (!FD_ISSET(socket[m_or_s]->sc_active_sockfd, &fd_list)) continue;

    unsigned char buffer[4096];
    //https://stackoverflow.com/questions/13433286/optimal-buffer-size-for-reading-file-in-c
    size_t sizeofbuffer=sizeof(buffer);//len_array == size_array for unsigned char.
    int read_bytes;
    bool use_spare=false;
    errno= socket[m_or_s]->Receive(buffer, sizeofbuffer, read_bytes, use_spare);

    if (read_bytes>0) buf_socket[m_or_s]->char_pushback(buffer,read_bytes);
      std::cout<< "Get_Buffer_Size(): "<<buf_socket[m_or_s]->Get_Buffer_Size()<<"\n";
    if (read_bytes<0)
    {
      std::string errormsg ;
      errormsg= std::string(std::strerror(errno));
      std::cout << port[m_or_s] << ":  " << errormsg << std::endl;
    }
  }
 
}


void LORA_STATION_V2::Interpret_And_Store_Incoming_Msgs(tm& rs_time)
{
  //printf("Value of header_bit: Hex: %X, Decimal: %d\n",header_bit,header_bit);
  
 for (int im_or_s=0; im_or_s<1; ++im_or_s) //master and slave loop. //katie: no m%s
  {
    std::string m_or_s = (im_or_s==0) ? "Master" : "Slave" ;
    int bufsize=buf_socket[im_or_s]->Get_Buffer_Size();
    //unpack all the msgs in this buffer.
    //earlier we only unpacked one msg in single looop run of main()
    while (bufsize>=min_msg_size)
    {
      std::vector<unsigned char> msg;
      msg = buf_socket[im_or_s]->extract_first_msg_in_buf(msg_header_bit,
                                                    msg_tail_bit,
                                                    idbit_and_msgsize);
      if (msg.size()==0) break; // break while() if no msg was found in buffer.
        std::cout<<"message received: "<<msg[1]<<"\n";

      most_recent_msg_rcvd_time[im_or_s]=Get_Current_Time_chrono();
    
      if (msg[1]==onesec_msg_bit)//Interpret
      {
          std::cout<<"found one sec\n";
          Unpack_OSM_Msg_Store_To_Spool(msg, im_or_s, rs_time); //Store
          Update_current_CTP(im_or_s); //after each OSM. Clock is separate for Master and Slave. //hershal
      }
      else if (msg[1]==event_msg_bit) //Interpret
      {
        std::cout<<"found event\n";
        Unpack_Event_Msg_Store_To_Spool(msg, m_or_s, im_or_s, rs_time); //Store
      }
      else if (msg[1]==cp_msg_bit)
      {
        std::cout <<"...log recvd..."<< name <<" " <<im_or_s << std::endl;
        Unpack_CP_Msg_Update_Threshold(msg, m_or_s); //Store
      }
      bufsize=buf_socket[im_or_s]->Get_Buffer_Size();
        
    }
  }
 
}

void LORA_STATION_V2::Send_Event_Spool_Info(tvec_EVENT_SPOOL_SUMMARY& v)
{
  
  bool all_spools_empty=true;
  for (int i=0; i<4; ++i)
  {
    if (event_spool[i].size()!=0)
    {
      all_spools_empty=false;
      break;
    }
  }
  if (all_spools_empty) return;

  for (int i=0; i<4; ++i)
  {
    for (int j=0; j<event_spool[i].size(); ++j)
    {
      if (event_spool[i][j].first.Total_counts==-99.0)
      {
        std::cout
        << "> > >  .... Process_Event_Spool_Before_Coinc_Check() not run. ........ < < <"
        << std::endl;
      }

      EVENT_SPOOL_SUMMARY temp;
      temp.station_name = name;
      temp.station_no = station_no;
      temp.evtspool_i = i;
      temp.evtspool_j = j;
      temp.Has_trigg = event_spool[i][j].first.Has_trigg;
      temp.time_stamp = event_spool[i][j].second;
      temp.GPS_time_stamp= event_spool[i][j].first.GPS_time_stamp;
      temp.nsec= event_spool[i][j].first.nsec;
      temp.charge = event_spool[i][j].first.Total_counts;
      temp.corrected_peak = event_spool[i][j].first.Corrected_peak;
      v.push_back(temp);
    }
  }
   
}

void LORA_STATION_V2::Discard_Events_From_Spool(const tvec_EVENT_SPOOL_SUMMARY& event_vec)
{
    
  for (int k=0; k<event_vec.size(); ++k)
  {
    if (name!=event_vec[k].station_name) continue;

    int i = event_vec[k].evtspool_i;
    int j = event_vec[k].evtspool_j;

    if (event_spool[i][j].second != event_vec[k].time_stamp) continue;

    event_spool[i].erase(event_spool[i].begin() + j);
  }
    
}

void LORA_STATION_V2::Send_Event_Data(tEvent_Data_Station& station_data,
                                      const tvec_EVENT_SPOOL_SUMMARY& event_vec)
{
  //Will send event_spool contents if this station is listed in event_vec
  //otherwise sends an empty station back.
  //this is called only if a station is active.
  //(obvious because inactive station class instance of LORA_STATION_V1 won't even be generated.)

  for (int i=0; i<4; ++i)
  {
    station_data.push_back(EVENT_DATA_STRUCTURE());
    station_data[i].detector = Get_Detector_Number(station_no,i);
  }

  int n_replaced=0;

  //if a channel is in the events list then its added to empty station
  //all 4 channels will be added mostly since even zero signal channel
  //should have a time stamp within 500ns. for 3 of 4 channels trig condition

  for (int k=0; k<event_vec.size(); ++k)
  {
    if (name!=event_vec[k].station_name) continue;

    int i = event_vec[k].evtspool_i;
    int j = event_vec[k].evtspool_j;

    if (event_spool[i][j].second != event_vec[k].time_stamp) continue;

    station_data[i]=event_spool[i][j].first;
    n_replaced+=1;
  }

  if (n_replaced!=4)
  {
    std::cout << "station did not contribute 4 ch's to event. " ;
    std::cout << n_replaced << " ch's were provided." << std::endl;
  }
    
}

void LORA_STATION_V2::Send_OSM_Delete_From_Spool(ONE_SEC_STRUCTURE& osm_master,
                                                 ONE_SEC_STRUCTURE& osm_slave)
{
  
  if (osm_spool[0].size()>0)
  {
    osm_master = osm_spool[0][0];
    osm_spool[0].erase(osm_spool[0].begin());
  }
  if (osm_spool[1].size()>0)
  {
    osm_slave = osm_spool[1][0];
    osm_spool[1].erase(osm_spool[1].begin());
  }
   
}

void LORA_STATION_V2::Send_Log(tLog_Station& noise_station_vec)
{
    /*
  for (int i=0; i<4; ++i)
  {
    float avg_mean = std::accumulate(hundred_means[i].begin(),
                                      hundred_means[i].end(),0.0);
    avg_mean /= hundred_means[i].size();

    float avg_sigma = std::accumulate(hundred_sigmas[i].begin(),
                                      hundred_sigmas[i].end(),0.0);
    avg_sigma /= hundred_sigmas[i].size();

    std::tm *t = Get_Current_Time();

    LOG_STRUCTURE temp;
    temp.detector = Get_Detector_Number(station_no,i);
    temp.YMD = (t->tm_year+1900)*10000 + (t->tm_mon+1)*100 + t->tm_mday ;
    temp.HMS = t->tm_hour*10000 + t->tm_min*100 + t->tm_sec;
    temp.GPS_time_stamp = (unsigned int)timegm(t);
    temp.Mean = avg_mean;
    temp.Sigma = avg_sigma;
    temp.Station = station_no;
    temp.Channel_thres_low = current_threshold_ADC[i];

    noise_station_vec.push_back(temp);
  }
     */
}

void LORA_STATION_V2::Calculate_New_Threshold(DETECTOR_CONFIG& dc,
                                              const tm& rs_time,
                                              int& current_reset_thresh_interval)
{
 /*
  int avg_over_dt= 60*60*1000; // ms

  std::tm *t = Get_Current_Time();
  short unsigned int year = t->tm_year + 1900;
  short unsigned int month = t->tm_mon + 1;
  short unsigned int day = t->tm_mday;
  short unsigned int hour = t->tm_hour;
  short unsigned int min = t->tm_min;
  short unsigned int sec = t->tm_sec;

  auto time_stamp_now=(long long unsigned)((year-rs_time.tm_year)*365*24*60*60);//to s
  time_stamp_now+=(long long unsigned)((month-rs_time.tm_mon)*30*24*60*60);//to s
  time_stamp_now+=(long long unsigned)((day-rs_time.tm_mday)*24*60*60);//to s
  time_stamp_now+=(long long unsigned)((hour-rs_time.tm_hour)*60*60);//to s
  time_stamp_now+=(long long unsigned)((min-rs_time.tm_min)*60);//to s
  time_stamp_now+=(long long unsigned)(sec-rs_time.tm_sec);
  time_stamp_now*=1000000000; //to ns

  for (int i=0; i<4; ++i)
  {
    int temp_detno=Get_Detector_Number(station_no,i);
    if (dc.calibration_mode==2)
    {
      auto vec= dc.list_of_dets_for_calib;
      auto det_in_list = std::find(vec.begin(), vec.end(), temp_detno)!= vec.end();
      if (det_in_list) continue;//do not alter threshold. this det is in calib mode.
    }

    //its 5 mins actually. not renaming vars again.
    float sixty_min_mean=0;
    float sixty_min_sigma=0;
    int sixty_min_n=0;

    Time_Average_From_Circ_Buffer(hundred_means[i],
                                  hundred_timestamps[i],
                                  time_stamp_now,
                                  avg_over_dt,
                                  sixty_min_mean, sixty_min_n);

    Time_Average_From_Circ_Buffer(hundred_sigmas[i],
                                  hundred_timestamps[i],
                                  time_stamp_now,
                                  avg_over_dt,
                                  sixty_min_sigma, sixty_min_n);

    if (sixty_min_n>0 && sixty_min_mean>0)
    {
      float temp = sixty_min_mean
                  + dc.sigma_ovr_thresh * sixty_min_sigma ;
      current_threshold_ADC[i]= static_cast<unsigned short>(temp);
    }
    else
    {
      current_threshold_ADC[i]-=2;
      current_reset_thresh_interval=dc.init_reset_thresh_interval;
      std::cout << "reducing thresh for " << name << " " << i << std::endl;
    }
  }
   */
}

void LORA_STATION_V2::Close()
{
    
	unsigned char dat[3] ;
	dat[0]=0x99 ;
	dat[1]=0xAA ;		//We have used identifier 'AA' for stopping DAQ in LORA
	dat[2]=0x66 ;
  auto lenofbuffer = sizeof(dat)/sizeof(dat[0]);
    //std::cout<<"check 3\n";
  for (int i=1;i<2;i++) //katie-> 1 for now to only use "send socket"
  {

    int bytes_sent = 0;
    bool use_spare_socket=false; //katie
    socket[i]->Send(dat,lenofbuffer,bytes_sent,use_spare_socket);

    socket[i]->Close();
  }
    //std::cout<<"check 4\n";

  sleep(2);
  std::stringstream command;
  //run the command on the detached screen session:
	command << "screen -S LORA" << station_no
          << " -X quit" ;

  std::cout << "Ending screen session: LORA" << station_no << std::endl;
	system(command.str().c_str());
 
  sleep(1);
    
}

void LORA_STATION_V2::Unpack_Event_Msg_Store_To_Spool(const std::vector<unsigned char>& msg,
                                                      const std::string& m_or_s,
                                                      const int& m_or_s_int,
                                                      const tm& rs_time)
{
  

  if (m_or_s!="Master" && m_or_s!="Slave")
  {
    std::stringstream ss;
    ss << "Unpack_Event_Msg_Store_To_Spool() got invalid input for m_or_s. 'Master' or 'Slave' allowed. ";
    ss << "\n . Received: " << m_or_s;
    throw std::runtime_error(ss.str());
  }
    std::cout<<"________________________event!________________________\n";


  EVENT_DATA_STRUCTURE event[4];  // for each channel
  
    
    
    
    
  unsigned char header = (unsigned char) msg[0];
  unsigned char identifier_bit = (unsigned char) msg[1] ;
  unsigned short byte_count = msg[3]<<8 | msg[2] ;
 // std::cout<<"byte count "<<byte_count<<"\n";

  uint16_t trigger_pattern = (unsigned char) msg[5]<<8 | msg[4] ;
    
  //std::cout<<"trigger pattern "<<std::bitset<16> (trigger_pattern)<<"\n";


    
  unsigned short year = msg[7]<<8 | msg[6] ;
  unsigned short month = msg[8] ;
  unsigned short day = msg[9] ;
  unsigned short hour = msg[10] ;
  unsigned short min = msg[11] ;
  unsigned short sec = msg[12] ;
  unsigned short status_elec = msg[13] ;
  std::cout<<"year, month, day, hour, min, sec: "<<year<<"  "<<month<<"  "<<day<<"  "<<hour<<"  "<<min<<"  "<<sec<<"\n";

  unsigned long CTD = (msg[17] & 0x7F)<<24 | msg[16]<<16 | msg[15]<<8 | msg[14] ;
  std::cout<<"CTD: "<<CTD<<"\n";
    
  unsigned short samples_ch1 = msg[19]<<8 | msg[18] ;
  unsigned short samples_ch2 = msg[21]<<8 | msg[20] ;
  unsigned short samples_ch3 = msg[23]<<8 | msg[22] ;
  unsigned short samples_ch4 = msg[25]<<8 | msg[24] ;

  std::cout<<"samples: "<<samples_ch1<<"  "<<samples_ch2<<"  "<<samples_ch3<<"  "<<samples_ch4<<"\n";
    
    
  unsigned short T1_ch1 = msg[27]<<8 | msg[26] ;
  unsigned short T2_ch1 = msg[29]<<8 | msg[28] ;
    
  unsigned short T1_ch2 = msg[31]<<8 | msg[30] ;
  unsigned short T2_ch2 = msg[33]<<8 | msg[32] ;
    
  unsigned short T1_ch3 = msg[35]<<8 | msg[34] ;
  unsigned short T2_ch3 = msg[37]<<8 | msg[36] ;
    
  unsigned short T1_ch4 = msg[39]<<8 | msg[38] ;
  unsigned short T2_ch4 = msg[41]<<8 | msg[40] ;

    std::cout<<"T1 levels: "<<T1_ch1<<"  "<<T1_ch2<<"  "<<T1_ch3<<"  "<<T1_ch4<<"\n";
    std::cout<<"T2 levels: "<<T2_ch1<<"  "<<T2_ch2<<"  "<<T2_ch3<<"  "<<T2_ch4<<"\n";
    
  printf("looking for list1: %x %x %x %x %x %x %x %x %x %x %x %x \n",msg[42],msg[43],msg[44],msg[45],msg[46],msg[47],msg[48],msg[49],msg[50],msg[51],msg[52],msg[53]);
    
  printf("looking for list2: %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x \n",msg[54],msg[55],msg[56],msg[57],msg[58],msg[59],msg[60],msg[61],msg[62],msg[63],msg[64],msg[65],msg[66],msg[67],msg[68],msg[69]);
    


    
   int istart=70;
    int iend=0;
   int len_trace=samples_ch1;
   
   int end_adc=istart+len_trace*2*4;
    
    
    int ch_count=0;
    int ch_hold[4][5000];
    int iadc=0;
    int temp=0;
    int i=0;

    for(i=0;i<4;i++){
        ch_count=0;
        iend = istart+2*len_trace;
        //printf("Channel %d:",i+1);
        for(iadc=istart;iadc<iend;iadc+=2) {
            ch_hold[i][ch_count]=*(short *)&msg[iadc];
            event[i].counts[ch_count]=*(short *)&msg[iadc];;

            ch_count++;
        }
        istart = iend;
    }
   printf("%d  %d\n",iend,end_adc);
   printf("looking for end markers --> %x  %x\n",msg[end_adc],msg[end_adc+1]);
    
  
    printf("trigger condition: %d\n",Control_Mode_Messages[10]);
    
    
    
    
  
  unsigned int Trigg_condition= Control_Mode_Messages[10];
  
  
	unsigned int nsec=(unsigned int)((1.0*CTD/current_CTP[m_or_s_int])*(1000000000)) ;
    printf("ns timing:   %u   %u   %u\n",CTD,current_CTP[m_or_s_int],nsec);

  //-----------xxx--------------
   tm t; t.tm_sec=sec ; t.tm_min=min ;
	t.tm_hour=hour ; t.tm_mday=day ;
	t.tm_mon=month -1 ; t.tm_year=year-1900 ;
  //Because GPS month starts from 1 while in 'tm' struct, it starts from 0.
	unsigned int GPS_time_stamp=(unsigned int)timegm(&t);
    

    
    
    std::cout<<"detector name: "<<name<<"\n";
  for (int i=0;i<4;++i)
  {
    auto detno = Get_Detector_NumberV2(name,i);

    event[i].Station = station_no ;
    event[i].detector = detno ;
    event[i].YMD = year*10000 + month*100 + day ;
    //GPS_time_stamp + 1 because LORA clock is behind 1 second. see old daq.
  	event[i].GPS_time_stamp = GPS_time_stamp +1 -18; //maybe 18 seconds off?
  	event[i].CTD = CTD;
  	event[i].nsec = nsec;
  	event[i].Trigg_condition = Trigg_condition;
    event[i].Trigg_pattern = trigger_pattern;
  	event[i].Total_counts  = -99.0;
  	event[i].Corrected_peak = -99.0;
  	event[i].Raw_peak = 0;
  }

	
  
  // //--------Event time stamp in nanosecs (w.r.t run_start_time and is used
  //"only" for checking the coincidences)--------
  // The calculation below assumes:-> 1 year=365 days; 1 month=30 days;
  // 1 day=24 hrs; 1 hour=60 mins; 1 min=60 sec
  auto time_stamp=(long long unsigned)((year-rs_time.tm_year)*365*24*60*60);//to s
  time_stamp+=(long long unsigned)((month-rs_time.tm_mon)*30*24*60*60);//to s
  time_stamp+=(long long unsigned)((day-rs_time.tm_mday)*24*60*60);//to s
  time_stamp+=(long long unsigned)((hour-rs_time.tm_hour)*60*60);//to s
  time_stamp+=(long long unsigned)((min-rs_time.tm_min)*60);//to s
  time_stamp+=(long long unsigned)(sec-rs_time.tm_sec);
  time_stamp*=1000000000; //to ns
  time_stamp+=(long long unsigned)(nsec) ;

  for (int i=0;i<4;i++)
  {
   //removed m_or_s
    event_spool[i].push_back(std::make_pair(event[i],time_stamp));
    
  }
    
    std::cout<<"______________________________________________________\n";

}
 


void LORA_STATION_V2::Update_current_CTP(const int& m_or_s)
{
 
  if (osm_spool[m_or_s].size()==0)
  {
    std::string errormsg="No OSM MSG FOUND TO PULL CTP FROM...";
    throw std::runtime_error(errormsg);
  }
  //FIXIT: once i saw this error above being thrown. how? since its only
  //called after Unpack_OSM_Msg_Store_To_Spool is called.
  int last_element = osm_spool[m_or_s].size() -1 ;
  current_CTP[m_or_s] = osm_spool[m_or_s][last_element].CTP;
 
}



void LORA_STATION_V2::Unpack_OSM_Msg_Store_To_Spool(const std::vector<unsigned char>& msg,
                                   const int& m_or_s,
                                   tm& rs_time)
{
    //katie: there is also information about all controlled parameters stored in the PPS message
    
    
    
    //std::cout<<"unpacking one sec message\n";
  
    ONE_SEC_STRUCTURE osm;
    std::stringstream ss22(name.substr(4,1));
    ss22 >> osm.Lasa;


    unsigned short header = msg[0];
    unsigned short identifier_bit = msg[1] ;
    unsigned short size = msg[3]<<8 | msg[2] ;
    unsigned short year = msg[5]<<8 | msg[4] ;
    unsigned short month = msg[6] ;
    unsigned short day = msg[7] ;
    unsigned short hour = msg[8] ;
    unsigned short min = msg[9] ;
    unsigned short sec = msg[10] ;
    unsigned short status_elec = msg[11] ;
    unsigned int CTP = (msg[15] & 0x7F)<<24 | msg[14]<<16 | msg[13]<<8 | msg[12] ;
    unsigned short sync =(msg[15] & 0x80)>>7;
    float quant = (msg[19]& 0x7F)<<24 | msg[18]<<16 | msg[17]<<8 | msg[16] ;
    
    unsigned short UTC_offset = msg[21]<<8 | msg[20] ; //Time(UTC) = Time (GPS) - UTC_offset
    unsigned short timing_flags=msg[22] ;  // resolution-T GPS manual pg 77-81
    unsigned short decoding_status=msg[23] ;  // resolution-T GPS manual pg 77-81
    unsigned short trigger_rate=msg[25]<<8 | msg[24] ;
    
    

    /*
    std::cout<<"PPS header: "<<header<<"\n";
    std::cout<<"PPS ID: "<<identifier_bit<<"\n";

    std::cout<<"PPS size: "<<size<<"\n";
    std::cout<<"year, month, day, hour, min, sec: "<<year<<"  "<<month<<"  "<<day<<"  "<<hour<<"  "<<minute<<"  "<<second<<"\n";
    std::cout<<"elec status: "<<status_elec<<"\n";
    std::cout<<"CTP: "<<CTP<<"\n";//<<(unsigned short)(msg[15] & 0x7F)<<"  "<<(unsigned short)msg[14]<<"  "<<(unsigned short)msg[13]<<"  "<<(unsigned short)msg[12]<<"\n";
    ///std::cout<<"byte 15: "<<std::bitset<8>(msg[15])<<"\n";
    std::cout<<"CTP error: "<<err_CTP<<"\n";//std::bitset<8>(err_CTP)<<" --> "<<std::bitset<8>((err_CTP)>>7)<<"\n";
    std::cout<<"quant: "<<quant<<"\n";
    std::cout<<"UTC offset: "<<UTC_offset<<"\n";
    std::cout<<"Trigger rate: "<<trigger_rate<<"\n";

	//short unsigned int month=msg[3] ;
	//short unsigned int year=(msg[4]<<8)+msg[5] ;
     */
    
    
    
    
    
    osm.YMD = year*10000 + month*100 + day ;
	osm.CTP=CTP;
    osm.quant=quant;
    osm.sync=sync;
    osm.trigger_rate=trigger_rate;
    osm.UTC_offset=UTC_offset;


    tm t; t.tm_sec=sec ; t.tm_min=min ;
    t.tm_hour=hour ; t.tm_mday=day ;
	t.tm_mon=month-1 ; t.tm_year=year-1900 ;
	//Because GPS month starts from 1 while in 'tm' struct, it starts from 0.
	osm.GPS_time_stamp=(unsigned int)timegm(&t) ;
   //GPS_time_stamp + 1 because LORA clock is behind 1 second. see old daq.
    osm.GPS_time_stamp+=1;

    //printf("~*~*~*~*~*~*~  V2 CTP update %u %d\n",osm.CTP,osm.CTP);


  osm.Master_or_Slave=m_or_s;

  osm_spool[m_or_s].push_back(osm);

  if(rs_time.tm_year==0)
  {
    std::cout << "Setting the RUN START TIME...............!\n";
    if (year==0)
    {
      std::string errormsg="OSM msg has year==0...";
      throw std::runtime_error(errormsg);
    }
    t.tm_mon=month ;
    t.tm_year=year ;
    rs_time = t;
  }

}

std::string LORA_STATION_V2::Send_Name()
{
  return name;
}



 
void LORA_STATION_V2::Process_Event_Spool_Before_Coinc_Check(DETECTOR_CONFIG& det_config,
                                                            int& wait_another_iteration)
{
    
  // Calculate properties for all traces present
  // in the event_spool - before they are forwarded to OPERATIONS
  // class for event forming and eventually discarded.
    //std::cout<<"Processing event!\n";
 
  for (int i=0; i<4; ++i)
  {
    for (int j=0;j<event_spool[i].size();++j)
    {
        
      //check if this trace had already been processed.
      if (event_spool[i][j].first.Total_counts!=-99.0) continue;
        
        
      //redundant but keeping for later reference.
      // the following was being used in the old DAQ for counting
      // n detectors for trigger decision.
      //assign trig bit to each channel by reading ch0's trig_pattern
      // unsigned short chtrig[4];
      // chtrig[0]= (event_spool[0][j].first.Trigg_pattern)&0x1 ;
      // chtrig[1]=(event_spool[0][j].first.Trigg_pattern>>2)&0x1 ;
      // chtrig[2]=(event_spool[0][j].first.Trigg_pattern>>4)&0x1 ;
      // chtrig[3]=(event_spool[0][j].first.Trigg_pattern>>6)&0x1 ;
      // event_spool[i][j].first.Has_trigg = chtrig[i];
        // katie: new DAQ doesn't record triggering this way
        
       
      //process waveform to get baseline/peak/counts/etc.
      int *temp = event_spool[i][j].first.counts;
      std::vector<signed short> trace(temp, temp+4000);
      int wpre= det_config.wvfm_process_wpre_v2;
      int wpost = det_config.wvfm_process_wpost_v2;
      int offwtrace_length= det_config.wvfm_process_offtwlen_v2;
      int raw_peak;
      float baseline, corrected_peak, integrated_counts,offw_std;
      Process_WaveformV2(trace,wpre,wpost,offwtrace_length,raw_peak, baseline, corrected_peak,integrated_counts, offw_std);
      
      //use the actual waveform to re-evaulate if it would have triggered.
      //this way, even though the actual ADC count used by the Digitizer
      //for threshold is not known(we supply mV and we roughly know mV2ADC)
      if (raw_peak>=current_threshold_ADC[i])
      {
        event_spool[i][j].first.Has_trigg=1;
      }
      else
      {
        event_spool[i][j].first.Has_trigg=0;
      }

      event_spool[i][j].first.Threshold_ADC= (unsigned int) current_threshold_ADC[i];

      //Store calculated quantities to apt longterm variables.
      hundred_means[i].push_back(baseline);
      hundred_sigmas[i].push_back(offw_std);
      hundred_timestamps[i].push_back(event_spool[i][j].second);

      if (event_spool[i][j].first.Has_trigg==1)
      hundred_thresh_crossing_timestamps[i].push_back(event_spool[i][j].second);

      if (j==0)
        hundred_station_event_timestamps.push_back(event_spool[i][j].second);

      event_spool[i][j].first.Total_counts = integrated_counts;
      event_spool[i][j].first.Corrected_peak = corrected_peak;
      event_spool[i][j].first.Raw_peak = raw_peak;
        
    }
      
  }

  if (event_spool[0].size()!=event_spool[1].size() ||
      event_spool[0].size()!=event_spool[2].size() ||
      event_spool[0].size()!=event_spool[3].size() )
    {
      if (event_spool[0].size()<4 && event_spool[0].size()>0)
      {
        std::cout << "Event_Spools sizes,"
        << name << ":" << event_spool[0].size() << ","
        << event_spool[1].size() << "," << event_spool[2].size()
        << "," << event_spool[3].size() << std::endl;

        request_OPS_to_wait_another_iteration+=1;

        if (request_OPS_to_wait_another_iteration<=3)
        {
          wait_another_iteration=1;
          return;
        }
      }
    }

  //if request had been made 3 times already, set counter 0 and move on.
  // or if it has never been made before... set it 0 anyway.
  request_OPS_to_wait_another_iteration=0;
 
  return;
}

void LORA_STATION_V2::Unpack_CP_Msg_Update_Threshold(const std::vector<unsigned char>& msg,
                                  const std::string& m_or_s)
{//katie: doesn't exist
 
}

void LORA_STATION_V2::Reset_Electronics()
{
    /*
	unsigned char dat[3] ;
	dat[0]=0x99 ;
	dat[1]=0xff ;
	dat[2]=0x66 ;
  auto lenofbuffer = sizeof(dat)/sizeof(dat[0]);

  for (int i=0;i<2;i++)
  {
    int bytes_sent = 0;
    bool use_spare_socket=true;
    socket[i]->Send(dat,lenofbuffer,bytes_sent,use_spare_socket);
    std::cout << "Reset Electronics Msg Sent. Size: " << bytes_sent << std::endl;
  }
    */
}

void LORA_STATION_V2::Print_Detectors_Diagnostics(const std::string& diagnostics_fname,
                                        const DETECTOR_CONFIG& dcfg,
                                        const tm& rs_time)
{
    
  std::ofstream outfile;
  std::tm *t = Get_Current_Time();
  unsigned int GPS_time_stamp=(unsigned int)timegm(t) ;

  short unsigned int year = t->tm_year + 1900;
  short unsigned int month = t->tm_mon + 1;
  short unsigned int day = t->tm_mday;
  short unsigned int hour = t->tm_hour;
  short unsigned int min = t->tm_min;
  short unsigned int sec = t->tm_sec;

  auto time_stamp_now=(long long unsigned)((year-rs_time.tm_year)*365*24*60*60);//to s
  time_stamp_now+=(long long unsigned)((month-rs_time.tm_mon)*30*24*60*60);//to s
  time_stamp_now+=(long long unsigned)((day-rs_time.tm_mday)*24*60*60);//to s
  time_stamp_now+=(long long unsigned)((hour-rs_time.tm_hour)*60*60);//to s
  time_stamp_now+=(long long unsigned)((min-rs_time.tm_min)*60);//to s
  time_stamp_now+=(long long unsigned)(sec-rs_time.tm_sec);
  time_stamp_now*=1000000000; //to ns

  outfile.open(diagnostics_fname,std::fstream::app);
  int avg_over_dt= 5*60*1000; // ms

  for (int i=0; i<4; ++i)
  {
    //its 5 mins actually. not renaming vars again.
    float ten_min_mean=0;
    float ten_min_sigma=0;
    int ten_min_n=0;
    int ten_min_n_thresh=0;

    Time_Average_From_Circ_Buffer(hundred_means[i],
                                  hundred_timestamps[i],
                                  time_stamp_now,
                                  avg_over_dt,
                                  ten_min_mean, ten_min_n);

    Time_Average_From_Circ_Buffer(hundred_sigmas[i],
                                  hundred_timestamps[i],
                                  time_stamp_now,
                                  avg_over_dt,
                                  ten_min_sigma, ten_min_n);

    Time_Average_From_Circ_Buffer(hundred_thresh_crossing_timestamps[i],
                                  time_stamp_now,
                                  avg_over_dt,
                                  ten_min_n_thresh);

    outfile << GPS_time_stamp
    << "\t" << Get_Detector_Number(station_no,i)
    << "\t" << ten_min_mean
    << "\t" << ten_min_sigma
    << "\t" << ten_min_n_thresh
    << "\t" << current_threshold_ADC[i]
    << std::endl;
  }

  outfile.close();
 
  return;
}

void LORA_STATION_V2::Set_New_Threshold()
{
  /*
  for (int i=0; i<1; ++i)//katie
  {
    std::cout << name << ", " << i << ": " << current_threshold_ADC[0+2*i] << " "
    << current_threshold_ADC[1+2*i] << std::endl;
    //mVolts2ADC=2.0 , that is the 2.0 factor below.
    unsigned char dat[7] ;
    dat[0]=0x99 ;
    dat[1]=0x77 ;		//We have use identifier '77' for the thresholds
    dat[2]=((unsigned short)(current_threshold_ADC[0+2*i]/(2.0*0.4883)) & 0xff00)>>8 ;
    dat[3]=((unsigned short)(current_threshold_ADC[0+2*i]/(2.0*0.4883)) & 0x00ff) ;
    dat[4]=((unsigned short)(current_threshold_ADC[1+2*i]/(2.0*0.4883)) & 0xff00)>>8 ;
    dat[5]=((unsigned short)(current_threshold_ADC[1+2*i]/(2.0*0.4883)) & 0x00ff) ;
    dat[6]=0x66 ;
    auto lenofbuffer = sizeof(dat)/sizeof(dat[0]);
    int bytes_sent;
    bool use_spare=true;
    socket[i]->Send(dat,lenofbuffer, bytes_sent, use_spare);
  }
  */
}

void LORA_STATION_V2::Status_Monitoring(bool& fatal_error, std::string& error_msg)
{
  //fatal error is usually false before being passed to this method.
  //can be true if another instance of LORA_STATION_V1 had set it to true
  //before this station gets it. Then this station will provide additional
  //error_msg if any.
    
  for (int i=0; i<2; ++i)
  {
    if (buf_socket[i]->Is_Buffer_Full())
    {
      fatal_error=true;
      std::stringstream ss;
      ss << name << " buf_socket[" << i <<  "] is full \n" ;
      error_msg += ss.str();
    }

    if (osm_spool[i].size()>300)
    {
      fatal_error=true;
      std::stringstream ss;
      ss << name << " osm_spool[" << i <<  "].size() =" << osm_spool[i].size()  ;
      error_msg += ss.str();
    }

    int dt_ms = Get_ms_Elapsed_Since(most_recent_msg_rcvd_time[i]);
    if (dt_ms >= 1*60*1000)//for one minute. at least 60 OSM msgs are missed!!
    {
      fatal_error=true;
      std::stringstream ss;
      ss << name << " " << i << " : No msgs received from this socket in "
      << dt_ms/1000 << "seconds." ;
      error_msg += ss.str();
    }
  }

  for (int i=0; i<4; ++i)
  {
    if (event_spool[i].size()>250)
    {
      fatal_error=true;
      std::stringstream ss;
      ss << name << " event_spool[" << i <<  "].size() =" << event_spool[i].size()  ;
      error_msg += ss.str();
    }
  }
     
}

int LORA_STATION_V2::Get_Sum_Size_of_Spools()
{
  int sum=0;
  for (int i=0; i<4; ++i) sum+=event_spool[i].size();
  for (int i=0; i<2; ++i) sum+=osm_spool[i].size();
  return sum;
}
// Vestigial code.

// void LORA_STATION_V1::Send_CP_Delete_From_Spool(tControl_Params_Station& cp_station)
// {//we don't save complete cp msgs since all info is same except threshold.
//   for (int i=0;i<4;++i)
//   {
//     if (cp_spool[i].size()>0)
//     {
//       cp_station.push_back(cp_spool[i][0]);
//       cp_spool[i].erase(cp_spool[i].begin());
//     }
//     else
//     {
//       cp_station.push_back(CONTROL_STRUCTURE());
//     }
//   }
// }


