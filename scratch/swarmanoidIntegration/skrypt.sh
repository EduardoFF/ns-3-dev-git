#!/bin/bash
cd /ns3/ns-allinone-3.12.1/ns-3.12.1
 ./waf --run "scratch/swarmanoidIntegration/swarmanoidIntegration scratch/swarmanoidIntegration/default.conf" &
sleep 5
cd /home/mkudelsk/IDSIA/ARGOS/argos2-source-20110818/build/user/marco
  launch_argos -nc xmls/wifiTest1.xml &
wait
