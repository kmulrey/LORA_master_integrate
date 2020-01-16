#/bin/bash
# to be run from lora main pc.
while true; do
  #rsync -vhr --ignore-times --checksum -e "ssh -p 2222" ../build/*.log hpandya@lofar-cr.iihe.ac.be:/user/hpandya/lora_daq/monitoring/logfiles/
  rsync -vhr --ignore-times --checksum -e "ssh -p 2222" ../normal_daq_output/*.log hpandya@lofar-cr.iihe.ac.be:/user/hpandya/lora_daq/monitoring/logfiles/normal_daq_output/
  rsync -vhr --ignore-times --checksum -e "ssh -p 2222" ../muon_calib/*.log hpandya@lofar-cr.iihe.ac.be:/user/hpandya/lora_daq/monitoring/logfiles/muon_calib/
  sleep 5
done
