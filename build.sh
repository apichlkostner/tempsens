#!/bin/sh

. /opt/poky/4.2.1/environment-setup-cortexa7t2hf-neon-vfpv4-poky-linux-gnueabi
mkdir build
cd build
cmake ..
make