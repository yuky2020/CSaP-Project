#!/bin/bash
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

