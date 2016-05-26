#!/bin/bash
if [ $# == 2 ] ; then
	./waf --run "swarmanoidIntegration $1" --cwd="/home/cinus/repos/ns-3-allinone/ns-3.9/scratch/swarmanoidIntegration" &> $2
elif [ $1 == "gdb" ] ; then
	./waf --run "swarmanoidIntegration" --cwd="/home/cinus/repos/ns-3-allinone/ns-3.9/scratch/swarmanoidIntegration"  --command-template="ddd %s"
else 
	./waf --run "swarmanoidIntegration $1" --cwd="/home/cinus/repos/ns-3-allinone/ns-3.9/scratch/swarmanoidIntegration"
fi
