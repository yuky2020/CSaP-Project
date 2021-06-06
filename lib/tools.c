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
#include<errno.h>

//functions to send an int trought a c stream socket
int send_int(int num, int fd)
{
    int32_t conv = htonl(num);
    char *data = (char*)&conv;
    int left = sizeof(conv);
    int rc;
    do {
        rc = write(fd, data, left);
        if (rc < 0) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                // use select() or epoll() to wait for the socket to be writable again
            }
            else if (errno != EINTR) {
                return -1;
            }
        }
        else {
            data += rc;
            left -= rc;
        }
    }
    while (left > 0);
    return 0;
}
//function to recive an int from a c socket 
int receive_int(int *num, int fd)
{
    int32_t ret;
    char *data = (char*)&ret;
    int left = sizeof(ret);
    int rc;
    do {
        rc = read(fd, data, left);
        if (rc <= 0) { /* instead of ret */
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                // use select() or epoll() to wait for the socket to be readable again
            }
            else if (errno != EINTR) {
                return -1;
            }
        }
        else {
            data += rc;
            left -= rc;
        }
    }
    while (left > 0);
    *num = ntohl(ret);
    return 0;
}



//function that give an int32 value from a PackageData the return is int32 insted of long or Byte(i dont use hard coded algorithhm like sha )  for be more light 
int32_t hashCode(PackageData tohash){

  int32_t i,ret;//return value
  for(i=0;i<strlen(tohash.from);i++){
    ret=ret+((int)tohash.from[i]+3*(int)tohash.to[i]);}//33 is the famus magic number
    ret=ret+tohash.message.maxFrameIndex;
  return ret;
  }


  int send_PackageData(PackageData tosend,int fd){
    int rvalue=0; // the return falue from mds if eevrything as gone fine is 0:
    //write the type  to MDS
    if (send_int(tosend.type,fd)<0) {
	      perror("write");
	      return 1;}
    //write  the user from the data is sended    
     if (send_int(strlen(tosend.from),fd)<0) {
	      perror("write");
	      return 1;}
     
    if (write(fd,tosend.from,strlen(tosend.from)+1)<0) {
	      perror("write");
	      return 1;}
    //write the user to the message is sended
    if (send_int(strlen(tosend.to),fd)<0) {
	      perror("write");
	      return 1;}
    if (write(fd,tosend.to,strlen(tosend.to)+1)<0) {
	    perror("write");
	    return 1;}
    //write the size of audio data
    if (send_int(tosend.size,fd)<0) {
	      perror("write");
	      return 1;}
    //write the audioData 
    /*if (write(fd,&tosend.message,tosend.size)<0) {
	      perror("write");
	      return 1;}
    */
   if(send_int(tosend.message.frameIndex,fd)<0){
     perror("write");
     return 1;}
   //saing sapce is always a good idea  
   if(send_int(tosend.message.maxFrameIndex/1024,fd)<0){
     perror("write");
     return 1;} 

    for(int i=0;i<(tosend.message.maxFrameIndex/1024);i++){
        if (write(fd,&tosend.message.recordedSamples[i],sizeof(SAMPLE))<0) {
	        perror("write");
	        return 1;}

    }
   
    //write the hash
    if (send_int(tosend.hash,fd)<0) {
	      perror("write");
	      return 1;}
    //write timestamp
    if (send_int(strlen(tosend.timestamp),fd)<0) {
	      perror("write");
	      return 1;}
    if (write(fd,tosend.timestamp,strlen(tosend.timestamp)+1)<0) {
	      perror("write");
	      return 1;}
    //check that everything was fine 
    if (receive_int(&rvalue,fd)<0) {
	      perror("write");
	      return 1;}

    return rvalue;
  }

  int recive_PackageData(PackageData *toreciv,int fd){
    int fromLenght;
    int toLenght;
    int timestampL;//lenght of the timestamp
    int rvalue=0;// value to return in case of success
    toreciv->type=6;

    //read the user from the data is sended
     if (receive_int(&fromLenght,fd)<0) {
	      perror("recive int ");
	      return 1;} 
    if (read(fd,&toreciv->from,fromLenght+1)<0) {
         perror("read");
	      return 1;}    
    toreciv->from[fromLenght] = '\0';  /* Terminate the string! */    
    //read the user to the message is sended
    //frist the lenght
    if (receive_int(&toLenght,fd)<0) {
	      perror("recive int ");
	      return 1;}
    if (read(fd,&toreciv->to,toLenght+1)<0) {
	      perror("read");
	      return 1;}
    toreciv->to[toLenght] = '\0';  /* Terminate the string! */      
    //read the size of audio data
    if (receive_int(&toreciv->size,fd)<0) {
	      perror("read");
	      return 1;}
    //read the audioData 
    /*if (read(fd,&toreciv->message,toreciv->size)<0) {
	      perror("read");
	      return 1;}
    */
     if(receive_int(&toreciv->message.frameIndex,fd)<0){
     perror("read");
     return 1;}
   if(receive_int(&toreciv->message.maxFrameIndex,fd)<0){
     perror("read");
     return 1;}
     toreciv->message.recordedSamples=malloc(toreciv->message.maxFrameIndex*sizeof(float)); 
    for(int i=0;i<(toreciv->message.maxFrameIndex);i++){
        if (read(fd,&toreciv->message.recordedSamples[i],sizeof(SAMPLE))<0) {
	        perror("read");
	        return 1;}

    }    

    //read the hash
    if (receive_int(&toreciv->hash,fd)<0) {
	      perror("read");
	      return 1;}
    //read timestamp
    if (receive_int(&timestampL,fd)<0) {
	      perror("read");
	      return 1;}
    if (read(fd,&toreciv->timestamp,timestampL+1)<0) {
	      perror("read");
	      return 1;}
    toreciv->timestamp[timestampL] = '\0';  /* Terminate the string! */    

    //check if the hash is still valid 
    printf("%s,%s\n",toreciv->to,toreciv->from);
    printf("%d",toreciv->hash);
    toreciv->message.maxFrameIndex=toreciv->message.maxFrameIndex*1024;
    if(toreciv->hash!=hashCode(*toreciv)){
	      perror("hash is different");
	      return 1;}
    //check if the username is =to the from ;
    //if (strcmp(username,tosend.from)==0){
	 //     perror("sombody is try to send a message as anoter user");
	 //       return 1;}
    //check that everything was fine 
    if (send_int(rvalue,fd)<0) {
	      perror("write");
	      return 1;}
   printf("Message sended");     
   return 0;

  }


//compare between two date in asctime format  return 1 if a>b 0 if b=a -1 if a<b using the asci value of number in predeterminated position 
int datecmp(char a[TIMESTAMPS],char b[TIMESTAMPS]){
  int ma,mb; //mounth a and b;
  if((int)a[22]>(int)b[22])return 1; //check millenio 
  else if((int)a[22]<(int)b[22])return -1;
 if((int)a[23]>(int)b[23])return 1; //check hundreds of years 
 else if((int)a[23]<(int)b[23])return -1;
 if((int)a[24]>(int)b[24])return 1; //check decades 
 else if((int)a[24]<(int)b[24])return -1;
 if((int)a[25]>(int)b[25])return 1; //check  years 
 else if((int)a[25]<(int)b[25])return -1;
 //now check mounth
 //Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
  if( (int)a[5]==(int)"J"){
    if(a[6]=="a")ma=1;
    else{if(a[7]=="n")ma=6;
	  ma=7;}
    }
  if((int)a[5]=="F")ma=2;
  if((int)a[5]=="M"){
    if((int)a[7]=="r")ma=3;
    else ma=5;
  }
  if((int)a[5]=="A"){
    if(a[6]=="p")ma=4;
    else ma=8;
  }
  if((int)a[5]=="S")ma=9;
  if((int)a[5]=="O")ma=10;
  if((int)a[5]=="N")ma=11;
  if((int)a[5]=="D")ma=12;

  if((int)b[5]=="J"){
    if((int)b[6]=="a")mb=1;
    else{if((int)b[7]=="n")mb=6;
	  mb=7;}
    }
  if((int)b[5]=="F")mb=2;
  if((int)b[5]=="M"){
    if((int)b[7]=="r")mb=3;
    else mb=5;
  }
  if((int)b[5]=="A"){
    if((int)b[6]=="p")mb=4;
    else mb=8;
  }
  if((int)b[5]=="S")mb=9;
  if((int)b[5]=="O")mb=10;
  if((int)b[5]=="N")mb=11;
  if((int)b[5]=="D")mb=12;
//finaly i have got the months for both
  if(ma>mb)return 1;
  if(mb>ma)return -1;

//looking for the day
  if((int)a[10]>(int)b[10])return 1;
  if((int)a[10]<(int)b[10])return -1;
  if((int)a[11]>(int)b[11])return 1;
  if((int)a[11]<(int)b[11])return -1;
//at this point we have the same date so return 0
  return 0;
}

