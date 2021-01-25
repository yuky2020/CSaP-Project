# CSaP Project: A AudioMessagge  application

Database-Server,Client-Server approch 

Fully writen in C (Tested on ArchLinux) .

## Parts:

Client:Interact with the user;

VDR: VoiceDataRepository,is aim is to store and update list of message of various user in an efficent way 

MDS:Main server, interact with the user for authentication and operate between Client and VDR as intermediary

## Features

Use PortAudio library (insted of Alsamixer or Pulseaudio ones) for better compatibility.

Auto-crop silence in the message by analyzing peek frequency.

Possibility of search for messages from a specific user, a date range and even the two things tougheter!

Possibility to send long audio without death times ;

No TIme limit for audio rec has still reconized ;

Huge use of semaphore and parent-child processes(use POSIX semaphore insted of system five one)..

The project is highly scalable(you can add how much VDR you want ), but can still improve(es. Decentralized Mdss that can comunicate each other  )



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
