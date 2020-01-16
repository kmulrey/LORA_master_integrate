#pragma once
#include <string>
#include <ctime>
#include <cstdio>
#include "Structs.h"
#include <boost/circular_buffer.hpp>

tchrono Get_Current_Time_chrono();

int Get_ms_Elapsed_Since(tchrono);

std::tm* Get_Current_Time();

unsigned long int Get_s_Since_Epoch();

unsigned int Get_Detector_Number(std::string, std::string, int);
unsigned int Get_Detector_Number(int, int);

char Trigger_Condition(unsigned short);

void Build_Hisparc_Messages(const unsigned short*, unsigned char*);

void Check_HV(const unsigned short,std::string);

void Process_Waveform(const std::vector<unsigned short>&,
                      const int&, const int&, const int&,
                      int&, float&, float&, float&, float&);

void Time_Average_From_Circ_Buffer(const boost::circular_buffer<float>&,
                                   const boost::circular_buffer<long long unsigned>&,
                                   const long long unsigned&,
                                   const int&,
                                   float&, int&);

void Time_Average_From_Circ_Buffer(const boost::circular_buffer<long long unsigned>&,
                                  const long long unsigned&,
                                  const int&, int&);
