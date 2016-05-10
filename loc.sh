#!/bin/bash

cat /dev/ttyAMA0 | awk '
$0 ~ /$GPG/ { print "lol" }
'

