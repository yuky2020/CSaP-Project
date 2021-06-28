#!/bin/bash
cd Mds
make clean
cd ..
cd Vdr 
make clean
cd ..
cd Client 
make clean
cd ..
rm dev/shm/sem.*
