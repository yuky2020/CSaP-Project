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
//function that give an int value from a PackageData the return is int insted of long for be more light
//max value of an int is 2,147,483,647 using a well tested algorithm like djb2 is usaly not enough for PackageData 
int hashCode(PackageData tohash){

  int i,ret;//return value
  for(i=0;i<MAXLIMIT;i++){
    ret=ret+((int)tohash.from[i]+33*(int)tohash.to);}//33 is the famus magic number
    ret=ret+tohash.size;
  for(i=0;i<TIMESTAMPS;i++)ret=ret+((int)tohash.timestamp[i]);

  }

//compare between two date in asctime format  return 1 if a>b 0 if b=a -1 if a<b using the asci value of number in predeterminated position 
int datecmp(char a[TIMESTAMPS],char b[TIMESTAMPS]){
  int ma,mb; //mounth a and b;
  if(a[22]>b[22])return 1; //check millenio 
  else if(a[22]<b[22])return -1;
 if(a[23]>b[23])return 1; //check hundreds of years 
  else if(a[23]<b[23])return -1;
 if(a[24]>b[24])return 1; //check decades 
  else if(a[24]<b[24])return -1;
 if(a[25]>b[25])return 1; //check  years 
  else if(a[25]<b[25])return -1;
 //now check mounth
 //Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
  if(a[5]=="J"){
    if(a[6]=="a")ma=1;
    else{if(a[7]=="n")ma=6;
	  ma=7;}
    }
  if(a[5]=="F")ma=2;
  if(a[5]=="M"){
    if(a[7]=="r")ma=3;
    else ma=5;
  }
  if(a[5]=="A"){
    if(a[6]=="p")ma=4;
    else ma=8;
  }
  if(a[5]=="S")ma=9;
  if(a[5]=="O")ma=10;
  if(a[5]=="N")ma=11;
  if(a[5]=="D")ma=12;

  if(b[5]=="J"){
    if(b[6]=="a")mb=1;
    else{if(b[7]=="n")mb=6;
	  mb=7;}
    }
  if(b[5]=="F")mb=2;
  if(b[5]=="M"){
    if(b[7]=="r")mb=3;
    else mb=5;
  }
  if(b[5]=="A"){
    if(b[6]=="p")mb=4;
    else mb=8;
  }
  if(b[5]=="S")mb=9;
  if(b[5]=="O")mb=10;
  if(b[5]=="N")mb=11;
  if(b[5]=="D")mb=12;
//finaly i have got the months for both
  if(ma>mb)return 1;
  if(mb>ma)return -1;

//looking for the day
  if(a[10]>b[10])return 1;
  if(a[10<b[10])return -1;
  if(a[11]>b[11])return 1;
  if(a[11]<b[11])return -1;
//at this point we have the same date so return 0
  return 0;
}

