#include "OPERATIONS.h"
#include <iostream>
#include "functions_library.h"
#include <sstream>

//This is basically the same script as main.cxx
//But with modules not needed for detector calib removed.
//And looping to start and stop daq for 20 panels.

int main()
{
  int run_time = 7;//in minutes
  //this whole run will take: 20 (sig) * 5 (sta) * 4 (ch) * 7 mins = 46.67 hrs
  for (float sigma_ovr_thresh=5.0; sigma_ovr_thresh<=7.4; sigma_ovr_thresh+=0.12)
  {
  for (int calib_ch=1; calib_ch<=1; calib_ch++)
  {
  for (int sta=5; sta<=5; ++sta)
  {
  std::cout << "------------------------x x x x x-----------------" << std::endl;
  std::stringstream this_detconfig_file, this_inticp_file;
  this_detconfig_file << "../input/detector_config_calib_lasa" << sta << ".txt";
  this_inticp_file << "../input/init_control_params_muon_calib_ch"
  << calib_ch << ".txt";
  std::vector<int> list_of_dets_for_calib;
  for (int i=0; i<4; ++i)
  {
    if (i==calib_ch-1) continue; //don't add the ch to be calibrated.
    list_of_dets_for_calib.push_back(Get_Detector_Number(sta,i));
  }

  OPERATIONS lora_daq;

  tchrono start_time = Get_Current_Time_chrono();

  lora_daq.Init("../input/network_config.txt",
           this_detconfig_file.str(),
           this_inticp_file.str(),
           "../input/init_control_params.txt", //katie
           "../input/init_control_params.txt", //katie
           "../daq_managers/daq_execution_status_file.txt",
           "../input/detector_coord.txt");

  lora_daq.Fiddle_With_Det_Config(sigma_ovr_thresh,list_of_dets_for_calib);

  sleep(1);

  lora_daq.Connect_To_Stations();

  sleep(1);

  lora_daq.Accept_Connections_From_Stations();

  lora_daq.Send_Control_Params();

  bool stay_in_loop=true;
  while(stay_in_loop)
  {
    lora_daq.Listen_To_Stations();

    lora_daq.Interpret_And_Store_Incoming_Msgs();

    lora_daq.Check_Coinc_Store_Event_Send_LOFAR_Trigger(!stay_in_loop);

    lora_daq.Reset_Thresh_Store_Log();

    lora_daq.Store_OSM();

    lora_daq.Print_Detectors_Diagnostics();

    lora_daq.Status_Monitoring();

    int dt_ms = Get_ms_Elapsed_Since(start_time);
    if (!lora_daq.Is_DAQ_Execution_Status_True() || dt_ms>run_time*60*1000)
    {
      stay_in_loop=false;
      std::cout << "Ending DAQ...\n";
      sleep(1);//required to wait for any running processes.
    }
  }

  int n_msgs_unsaved = lora_daq.Get_Sum_Size_of_Spools();
  std::cout << std::endl << "Shutting the code... "
  << "emptying all recvd msgs from spools... " << std::endl;
  int count=0;
  while (n_msgs_unsaved>0 && count<2000)
  {
    if (count==400 || count==800 || count==1200) std::cout << "." ;
    lora_daq.Interpret_And_Store_Incoming_Msgs();
    lora_daq.Check_Coinc_Store_Event_Send_LOFAR_Trigger(!stay_in_loop);
    lora_daq.Store_OSM();
    lora_daq.Periodic_Store_Log();
    lora_daq.Print_Detectors_Diagnostics();
    int new_value = lora_daq.Get_Sum_Size_of_Spools();
    if (new_value==n_msgs_unsaved)
      count++;
    else count=0;
    n_msgs_unsaved = new_value;
  }
  std::cout << std::endl<< "All spools are empty... have a good day!" << std::endl;

  lora_daq.End();
  }
  }
  }
  return 0;
}
