#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include"util.h"
#define MAXLIMIT 20

int login(userData u, int t,int  s ){

  u.type=t; 
  //wirte in the socket data for login or register
      if (write(s,&u,sizeof(u))<0) {
	perror("write");
	exit(1);
    }

    // Read  from socket 1 for login or register done 0 for not 
    if (read(s,&t,sizeof(t))<0) {
	perror("read");
	exit(1); }

    return t;  

  }
int main(int argc, char *argv[])
{
 if (argc >3) {
	      printf("\nUsage:\n\t%s <User> <Password>\nOr no argument and wait for prompt\n",argv[0]);
	      exit(1);
              }
   
printf("____________________________________________");
printf("Welocome to AudioMessage ");
printf("--------------------------------------------- \n");
int s,t=0;
//inizialize userData
userData afantastic;
char hostname[MAXLIMIT];
struct sockaddr_in saddr;
struct hostent *hst;
char port[5]; //used to read and store  the port 
FILE *config;
//open the config file
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




if (argc==3){t=1;
             *afantastic.username=argv[1];
             *afantastic.password=argv[2];
            }

do{ if(t==0){printf("1)Login\n");
              printf("2)Register\n");
              scanf("%d",&t);
              getchar();
              printf("Please insert user:");
              fgets(afantastic.username, MAXLIMIT, stdin); 
              printf("\nPlease insert password:");
              fgets(afantastic.password, MAXLIMIT, stdin); }
                 
       if(!login(afantastic,t,s)){ t=0; printf("\nAuthFalied please try again\n");}
        }while(t==0);

printf("Welcome Back %s \n",afantastic.username );
printf("%d new messages for you \n",checkinbox(afantastic,s));


int i;
printf("1) listen  new message \n");
printf("2) send a new message \n");
printf("3) add new contact \n ");
scanf("%d",&i);
switch (i) {
    case 2 :
      {  AudioData audio = record();
        playback(audio);
       }
        
    default: printf("This option is not avaible \n"); 
        
}



}

