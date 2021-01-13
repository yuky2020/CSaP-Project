#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

void main(int argc, char *argv[])
{
    int s;
    struct sockaddr_in saddr;
    struct hostent *hst;
    int ops[3];
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

    // Connect to other socket
    if (connect(s,(struct sockaddr *)&saddr,sizeof(saddr))<0){
	perror("connect");
	exit(1);
    }
    puts("connect done");


    
    // Write (or send) to socket
    if (write(s,ops,sizeof(ops))<0) {
	perror("write");
	exit(1);
    }

    // Read (or recv) from socket
    if (read(s,ops,sizeof(ops))<0) {
	perror("read");
	exit(1);
    }

    printf("%d + %d = %d\n",ops[0],ops[1],ops[2]);

    // Close the client socket
    close(s);
}
