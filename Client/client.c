#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include"util.h"
#define MAXLIMIT 20

//list all mesage sended to this user contact the MDS to get data, is an interactive function 
int listallmessage(int s){
  int mdsRet;//return value from mds;
  int type=8;//the type of call for get all messages destinated to a user;
  int inboxn=checkinbox(s)//recall now because inbox number could change in small time 
  //PackageData *c=malloc(1*sizeof(PackageData));
  PackageData messageList[inboxn];
    //wirte in the socket data for login or register
  if (write(s,&type,sizeof(type))<0) {
	perror("write");
	return 1;
    }
  printf("##############");
  printf("INBOX\n");
  printf("##############");
  printf("SElECT MESSAGE");

  for(int i=0;i<inboxn;i++){
    if(read(s,&messageList[i].type,sizeof(messageList[i].type))<0){
       perror("read");
       return 1;}

    if (read(s,&messageList[i].from,sizeof(messageList[i].from))<0) {
	perror("read");
	return 1;}
    //read the user to the message is sended
    if (read(s,&messageList[i].to,sizeof(messageList[i].to))<0) {
	perror("read");
	return 1;}
    //read the size of audio data
    if (read(s,&messageList[i].size,sizeof(messageList[i].size))<0) {
	perror("read");
	return 1;}
    //read the audioData 
    if (read(s,&messageList[i].message,messageList[i].size)<0) {
	perror("read");
	return 1;}

    //read the hash
    if (read(s,&messageList[i].hash,sizeof(messageList[i].hash))<0) {
	perror("read");
	return 1;}
    //read timestamp
    if (read(s,&messageList[i].timestamp,sizeof(messageList[i].timestamp))<0) {
	perror("read");
	return 1;}
    //check if the hash is still valid 
    if(messageList[i].hash!=hashCode(messageList[i])){
	perror("hash is different");
	return 1;}
      
     }
  //after reding data read the return value 
  if (read(s,&mdsRet,sizeof(mdsRet))<0) {
	perror("read");
	return 1;}
  //check if is correct
  if(mdsRet!=5){
      perror("Sending hasent work");
      free(messageList);
      return 1;
    }
  //send doble check to Mds
  if (write(s,&mdsRet,sizeof(mdsRet))<0) {
	perror("read");
	free(messageList);
	return 1;}
    
  for(int i=(inboxn-1);i>=0;i--){//print like that for have the last message frist
    printf("%d)Message from %s sended %s\n",(inbox-i),messageList[i].from,messageList[i].timestamp);
  
    }


  
   

  


}
//send login data to socket return 1 for auth compleate 0 on failed
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

//return number of massage stored actualy in Inbox 
int checkinbox(s){
  int ret,type=7 //7 is the type for this call
    //write type in the socket
    if (write(s,&type,sizeof(type))<0) {
	perror("write");
	exit(1);
    }
    //then the socket should return the number of massage
    if (read(s,&ret,sizeof(t))<0) {
	perror("read");
	exit(1); }
  return ret;
}

int main(int argc, char *argv[])
{
 if (argc >3) {
	      printf("\nUsage:\n\t%s <User> <Password>\nOr no argument and wait for prompt\n",argv[0]);
	      exit(1);
              }
printf("\033[0;36m");   
printf("____________________________________________");
printf("Welocome to AudioMessage ");
printf("--------------------------------------------- \n");
printf(“\033[0m”); 
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
int i;
do{
  printf("%d inbox messages for you \n",checkinbox(afantastic,s));
  printf("1) show the new messages \n");
  printf("2) send a new message \n");
  printf("3) add new contact in the address book \n ");
  ptintf("9)EXIT\n");
  scanf("%d",&i);
  switch (i) {
    case 1:{
	   listallmessage(s);
	 }
    case 2 ://select a user and then send a message used can be select eihter by address book or by write is name ;
	 
      {PackageData tosend; 
	tosend.to=selectuserto(s);//select the user to send the data to pass the socket to the 
	tosend.message = recordP();
	tosend.size=sizeof(tosend.message);
	tosend.from=afantastic.username;
	time_t ltime; /* calendar time */
        ltime=time(NULL); /* get current cal time */
	tosend.timestamp=asctime( localtime(&ltime));//Get a timestamp of the actual moment in a redable format;
	tosend.hash=hashCode(PackageData);
        do{ printf("1) send the message\n");
	    printf("2) listen the message before send it\n");
	    printf("3) to return to the main menu\n");
	    printf("4)change the destinatary user actualy is: %s \n",tosend.to);
	    scanf("%d",&i);
	    if(i==1){
	      printf("sending your message \n");
	      if(sendMessage(tosend,s))printf("MESSAGE NOT SENT NETWORK PROBLEM TRY AGAIN \n");//sendMessage return 1 upon fail
	      else{printf("MESSAGE SENT SUCESSFULLY\n");
		 printf("1)go back to the main menu\n");
		 printf("2)inoltrate this massage to another user");
		 scanf("%d",&i);
		 i=i+3;//smart move in this way if you have pressed 1 you go back and if you press 2 you inoltrate the message
		  }
	      }
	    if(i==2)playback(tosend.message);//playback the audio registered
	    if (i==4){tosend.to=selectuserto(s);
		      tosend.hash=hashCode(tosend);
	      }
	  }while(i!=4);
      }//close case 2;
    case 3:{addusertoadressbook(s);}
    	
    	break;
        
    default: printf("This option is not avaible \n"); 
        
    }
  }while(i!=9);
}
