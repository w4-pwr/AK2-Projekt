#!/bin/bash
nohup sudo cat /dev/ttyAMA0 > output.txt &
cat output.txt | awk ' $0 ~ /GPGLL/ {print $0 }'
