#!/bin/bash
rm /dev/shm/sem.*
cd Mds
./MDS&
sleep 2
cd ..
cd Vdr
sleep 2
./VDR&
cd ..
cd Client
sleep 2
./CLIENT
