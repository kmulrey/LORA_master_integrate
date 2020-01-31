#!/bin/bash
echo "Start LORA $1"

ssh root@lora$1 "/root/test/main"

