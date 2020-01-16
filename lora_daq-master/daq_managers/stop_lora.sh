#!/bin/bash
sed '2 s/1/0/' daq_execution_status_file.txt > new_file.txt
mv new_file.txt daq_execution_status_file.txt
