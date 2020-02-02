#!/bin/bash
echo "Start LORA $1"
sleep 10
ssh root@lora$1 "cd /root/test/ && ./main"

