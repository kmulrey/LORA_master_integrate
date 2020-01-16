#!/bin/bash
#This script does the following:
# start daq , wait 48 hours, stop daq, do elec calib, store calib files
# cd /home/hpandya/lora_daq/daq_managers/
../build/main_muon_calib
while true; do
  # screen -S LORAMAIN -X stuff 'cd /home/hpandya/lora_daq/build/\n'
  screen -S LORAMAIN -X stuff './main\n'
  date
  sleep 172800
  sh stop_lora.sh
  sleep 40
  ../build/main_elec_calib
  now=$(date +"%m_%d_%Y")
  mv ../input/init_control_params.txt ../input/backup_init_control_params_$now.txt
  cp `ls -t ../electronics_calib/*txt | head -n 1` ../input/init_control_params.txt
  git pull
  git add ../input/init_control_params.txt ../input/backup_init_control_params_$now.txt `ls -t ../electronics_calib/*txt | head -n 1`
  git commit -m "update elec calib files"
  git push
done
