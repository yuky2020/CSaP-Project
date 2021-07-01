#!/bin/bash
#system type check

if [[ `cat /etc/os-release | grep VERSION_CODENAME` != 'VERSION_CODENAME=hirsute' && `cat /etc/os-release | grep VERSION_CODENAME` != 'VERSION_CODENAME=buster'  ]]; then
    echo "Must be on a Debian derivate to run this script"
    exit
fi
#more professional than before for sure 
# this part is for check if make and gcc are installed since vanilla version of debian could not include this build essental package
allReqPackage=("gcc" "make" "libjack-dev" "libsndfile-dev" "libasound2-dev")
for i in ${!allReqPackage[@]}; do
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' ${allReqPackage[$i]}|grep "install ok installed")
echo Checking for ${allReqPackage[$i]}: $PKG_OK
if [ "" = "$PKG_OK" ]; then
  echo "No ${allReqPackage[$i]}. Setting up ${allReqPackage[$i]}."
  sudo apt-get --yes install ${allReqPackage[$i]} 
fi
done

cd Mds
make 
cd .. 
cd Vdr
make
cd ..
cd Client
make 
cd ..

