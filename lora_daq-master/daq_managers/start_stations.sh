#!/bin/bash
echo "Start LORA $1"

ssh root@lora$1 "/opt/Satyendra/run/main_lasa outputM outputS"
