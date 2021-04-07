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



//function that give an int value from a PackageData the return is int insted of long for be more light
//max value of an int is 2,147,483,647 using a well tested algorithm like djb2 is usaly not enough for PackageData 
int hashCode(PackageData tohash){

  int i,ret;//return value
  for(i=0;i<MAXLIMIT;i++){
    ret=ret+((int)tohash.from[i]+33*(int)tohash.to[i]);}//33 is the famus magic number
    ret=ret+tohash.size;
  for(i=0;i<TIMESTAMPS;i++)ret=ret+((int)tohash.timestamp[i]);
  return ret;

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

