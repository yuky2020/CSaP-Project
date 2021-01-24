# CSaPProject

## Metadata Server (MDS)

### ● Handles initial connections with clients, maintains a “database” (even implemented as a file) containing

information regarding users, messages, mailbox status, free space, etc

### Upon startup, or after a crash, the MDS contacts all known VDR processes, which could be running on the

same or on another server, to get/check information about the status.

### – Both the MDS and VDR should keep copy of their own metadata.

### The information wrt IP addresses and port# of the various components is contained in a configuration file,read upon startup by all processes

### After initialization, the MDS waits for requests from clients which, upon authentication (using

name/password), could:
– Check for new messages.
– Add/delete users in their personal address book.
– Send (record) a voice message to another user (search from all users or pick up from address book).
– Search for messages from a specific user (selected as above) or within a certain date range.
– Listen, delete or forward a voicemail.

## ONE or more Voice Data Repositories (VDR)

### ● Store audio files, keeping a copy of metadata (username,

timestamp, ..) plus an hash for security.

### ● When starting, they check local files for corruption, rebuild their

metadata and “check-in” with MDS

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

### ● Record a voice message and send it to a user (selected as indicated in the

above bullet)

*XMind - Trial Version*