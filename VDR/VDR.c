#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include "util.h"
//function to store a Message
storeMessage(int s){
MessageData tostore;
if (read(s,&tostore.,sizeof(type))<0) {
	perror("write");
	exit(1);


}
void main(int argc, char *argv[])
{
    int s;
    struct sockaddr_in saddr;
    struct hostent *hst;
    int type;// indicates the type of request from MDS 1 is a echo request 2 is for a audio message 
    FILE *config;

//open config file 
if ((config = fopen("config.txt", "r")) == NULL) {
      perror(" opening file");
      // Program exits if file pointer returns NULL.
      exit(1);
    }
//skip the string hostname: 
fseek(config, 10*sizeof(char), SEEK_CUR); 
// reads text until newline is encountered
fscanf(config, "%[^\n]", hostname);
//skip also port:
fseek(config, 7*sizeof(char), SEEK_CUR);
//read port number
fscanf(config, "%[^\n]", port);
fclose(config);

// here for testing pourpose
 printf("%s,%s",hostname,port);





// Create the stream socket 
    if ((s=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP))<0) {
	perror("socket");
	exit(1);
    }
    puts("socket done");

    // Determine host address by its network name
    if ((hst=gethostbyname(hostname))==NULL) {
	perror("gethostbyname");
	exit(1);
    }
    printf("gethostbyname: %u %d\n",*(int*)hst->h_addr,hst->h_length);

    // Fill structure
    bcopy(hst->h_addr,&saddr.sin_addr,hst->h_length);
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(atoi(port));

    // Connect to other socket(mds)
    if (connect(s,(struct sockaddr *)&saddr,sizeof(saddr))<0){
	perror("connect");
	exit(1);
    }
    puts("connect done");

do{
    
    // Write (or send) to socket
    if (read(s,&type,sizeof(type))<0) {
	perror("write");
	exit(1);
    }
    //type 1 for echo
    if(type==1){
    // Read (or recv) from socket
    printf("echo call from Mds \n");
    //return the same value 1 for notifing that the Vdr is online ;
    if (write(s,&type,sizeof(type))<0) {
	perror("read");
	exit(1);
	}}
    //type six for message to store
    if(type==6){ 
	storeMessage(s);


    }
    

    
    }while(type!=5);
        // Close the client socket
    close(s);

}
