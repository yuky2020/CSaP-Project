# CSaP Project: A AudioMessagge  application

#### Database-Server,Client-Server approch

#### Fully writen in C (Tested on ArchLinux ,Pop OS 21.04,Parrot OS,kali linux ) .
#### Program is intended anyway to be used with debian Buster  and it's derivates (since  scripts are tested mostly on debian derivates Parrot os,Kali,Pop os 21.04(derviate of a derivate distro(Ubuntu)) 
##### A future Aur package for Arch is plasuible and  also .deb package. 

## Parts:

#### Client:Interact with the user;

#### VDR: VoiceDataRepository,is aim is to store and update list of message of various user(his inbox) in an efficent way.

#### MDS:Main server, interact with the user for authentication and operate between Client and VDR as intermediary.

## Features(Design Choices)

Use PortAudio library (insted of Alsamixer or Pulseaudio ones) for better inter-compatibility.

Auto-crop silence in the message by analyzing peek frequency.

Possibility of search for messages from a specific user, a date range and even the two things tougheter!

Possibility to send long audio without death times ,no time limit for audio rec has still reconized .

Socket operation are made atomical by design of specific function (no large buffers are used) for improve security. 

Huge use of semaphore and parent-child processes(use POSIX semaphore insted of system five one)..

The project is highly scalable(you can add how much VDRs you want ), but can still improve(es. Decentralized Mdss that can comunicate each other  ).

Every message is checked for corruption before every tranfer between VDR and MDS or MDS and CLIENT and vice versa.

possibility to remove all the inbox messages with one call

or to delate message one by one. 

change password for logged users implemented.

some security checks in the calls to prevent attacks using a modded version of the source code .

portaudio already present and  bundled you only need to install some audio related library .

80's style command line interface.

## What happened if a part fail(a part die for some external  reason )?

##### If mds fail the client is disconnected and vdr  close the connection and  waiting for the mds to reconnect

##### If a vdr fail the Mds will close the connection with the clients and die (no restart or use of goto to go before connection since is more easy to set up an auto restart system)

##### If a client fail well the  socket for that client  and child process in the mds are correctly closed and you can easily reconnect

## Dependencies:

all the dependencies nedded to use a bundled version of portaudio 

#### -ljack  libjack-dev

 sudo apt install  libjack-dev

#### -lsndfile

sudo apt-get  install libsndfile-dev

#### -lasound

sudo apt-get install libasound2-dev 

### How to Install?

an easy script is already here for debian users

### How to test ?

there is a script for everything in this project 

### How to clean

you already know 

### Release notable problems:

###### ~~lib-portaudio work using alsa on linux  and  sometime i found problems in getting audio by the  microphone off my laptop (Dell G3), anyway i have also  problems  with other app like zoom or google meet~~   (turns out that was *Intel* Sunrise Point-LP HD *Audio*  devicd and  is *Linux* driver snd_hda_intel related problem)

###### maybe recording is a little too verbose and config files too little

### Project   structure(each function and is use ) :

![](https://github.com/yuky2020/CSaP-Project/blob/main/Project%20structure/CSAP%20PROJECT.png?raw=true)

## Some of the project requirements

### Metadata Server (MDS)

###### ● Handles initial connections with clients, maintains a “database” (even implemented as a file) containinginformation regarding users, messages, mailbox status, free space, etc

###### Upon startup, or after a crash, the MDS contacts all known VDR processes, which could be running on thesame or on another server, to get/check information about the status.

###### – Both the MDS and VDR  keep copy of their own metadata.

###### The information for socket connections is contained in a configuration file,read upon startup by all processes

###### After initialization, the MDS waits for requests from clients

### 

## ONE or more Voice Data Repositories (VDR)

###### ● Store audio files, keeping a copy of metadata (username,timestamp, ..) plus an hash for security.

##### ● Send/Receive/Delete audio files upon request from MDS (checking hashes before returning them to server)

## 

## Clients

##### ● Connect to MDS to authenticate

##### ● Check for new messages

##### Search for messages:

– From specific users, selecting them from the entire list of from a personal
address book
– Within a specific data range

##### ● Retrieve and play messages on the local audio system

##### ● Delete messages

##### ● Record a voice message and send it to a user (selected as indicated in the above bullet)
