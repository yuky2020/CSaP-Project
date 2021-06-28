# CSaP Project: A AudioMessagge  application

#### Database-Server,Client-Server approch

#### Fully writen in C (Tested on ArchLinux ,Pop OS 21.04,Parrot OS,kali linux ) .

## Parts:

Client:Interact with the user;

VDR: VoiceDataRepository,is aim is to store and update list of message of various user(his inbox) in an efficent way. 

MDS:Main server, interact with the user for authentication and operate between Client and VDR as intermediary.

## Features

Use PortAudio library (insted of Alsamixer or Pulseaudio ones) for better inter-compatibility.

Auto-crop silence in the message by analyzing peek frequency.

Possibility of search for messages from a specific user, a date range and even the two things tougheter!

Possibility to send long audio without death times .

No Time limit for audio rec has still reconized .

Huge use of semaphore and parent-child processes(use POSIX semaphore insted of system five one)..

The project is highly scalable(you can add how much VDRs you want ), but can still improve(es. Decentralized Mdss that can comunicate each other  ).

Every message is checked for corruption before every tranfer between VDR and MDS or MDS and CLIENT and vice versa.

possibility to remove all the inbox messages with one call

or to delate message one by one. 

change password for logged users implemented.

some security checks in the calls to prevent attacks using a modded version of the source code .

portaudio already present and  bundled you only need to install some audio related library .

80's style command line interface.

### Requirents:

all the requirements nedded to use a bundled version of portaudio 

#### -ljack  libjack-dev

 sudo apt install  libjack-dev

#### -lsndfile

sudo apt-get  install libsndfile-dev

#### -lasound

sudo apt-get install libasound2-dev 

### Release notable problems:

###### ~~lib-portaudio work using alsa on linux  and  sometime i found problems in getting audio by the  microphone off my laptop (Dell G3), anyway i have also  problems  with other app like zoom or google meet~~   (turns out that was *Intel* Sunrise Point-LP HD *Audio*  devicd and  is *Linux* driver snd_hda_intel related problem)

###### maybe recording is a little too verbose and config files too little

### Project   structure :

![](https://github.com/yuky2020/CSaP-Project/blob/main/Project%20structure/CSAP%20PROJECT.png?raw=true)

### Metadata Server (MDS)

#### ● Handles initial connections with clients, maintains a “database” (even implemented as a file) containinginformation regarding users, messages, mailbox status, free space, etc

#### Upon startup, or after a crash, the MDS contacts all known VDR processes, which could be running on thesame or on another server, to get/check information about the status.

#### – Both the MDS and VDR should keep copy of their own metadata.

#### The information wrt IP addresses and port# of the various components is contained in a configuration file,read upon startup by all processes

#### After initialization, the MDS waits for requests from clients which, upon authentication (usingname/password), could:

### 

## ONE or more Voice Data Repositories (VDR)

### ● Store audio files, keeping a copy of metadata (username,timestamp, ..) plus an hash for security.

### ● When starting, they check local files for corruption, rebuild their metadata and “check-in” with MDS

### ● Send/Receive/Delete audio files upon request from MDS (checking hashes before returning them to server)

## 

## Clients

### ● Connect to MDS to authenticate

### ● Check for new messages

### Search for messages:

– From specific users, selecting them from the entire list of from a personal
address book
– Within a specific data range

### ● Retrieve and play messages on the local audio system

### ● Delete messages

### ● Record a voice message and send it to a user (selected as indicated in the above bullet)
