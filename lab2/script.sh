#!/bin/sh

wget https://inp111.zoolab.org/lab02.1/testcase.pak -O testcase.pak 2> /dev/null
rm -rf /tmp/testcase
mkdir /tmp/testcase
./unpacker testcase.pak /tmp/testcase > /dev/null
cd /tmp/testcase
chmod +x checker
./checker
