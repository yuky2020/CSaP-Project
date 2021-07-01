#!/bin/bash
#system type check

if [[ `cat /etc/os-release | grep VERSION_CODENAME` != 'VERSION_CODENAME=hirsute' && `cat /etc/os-release | grep VERSION_CODENAME` != 'VERSION_CODENAME=buster'  ]]; then
    echo "Must be on a Debian derivate to run this script"
    exit
fi

sudo apt install libjack-dev
sudo apt-get install libsndfile-dev
sudo apt-get install libasound2-dev
cd Mds
make 
cd .. 
cd Vdr
make
cd ..
cd Client
make 
cd ..

