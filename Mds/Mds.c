#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include "util.h"
#define VDRN 1 //Number of vdr used 
//registration of a user 
int registeru(userData afantasticuser){
   FILE *fp;

   // save the address of the file 
   char address[(MAXLIMIT+10)] = {'\0'};
   sprintf(address, "users/%s.user", afantasticuser.username);
    //check if the file alredy exist and if it is the case return 0
   if ((fp = fopen(address, "rb")))
    {
        fclose(fp);
        return 0;
    }
    //now open and  write on the file 
    fp = fopen(address,"wb");
    if(fp == NULL)
    {
      perror("file open");
      return 0;
      exit(1);             
    }

  
   fwrite(afantasticuser.password,sizeof(char[MAXLIMIT]),1,fp); 
   fclose(fp);
   
   return 1;
}
//autenticate a user 
int autenticate(userData afantasticuser){

    FILE *fp;

   // save the address of the file 
   char address[(MAXLIMIT+10)] = {'\0'};
   sprintf(address, "users/%s.user", afantasticuser.username);
    //check if the file alredy exist and if it is the case return 0
   fp = fopen(address, "rb");
   if(fp == NULL){
       perror("no such file");
       return 0;
   }
   char tmp[MAXLIMIT];
   fread(tmp,sizeof(tmp),1,fp);
   if(!strcmp (afantasticuser.password,tmp))return 1;
   else return 0;
   fclose(fp);
}




void dowork(int c,vdrs[VDRN])
{
    //@var User to login or register 
    userData afantasticuser;
    int type,result,tentatives=0;
    //client can try to acess five times then for security reason connection is abortefor security reason connection is abortedd
    //read from Stream Socket
    do{
    if (read (c, &afantasticuser, sizeof (userData)) < 0) {
	perror ("read");
	exit (1);
    }
    
   if(afantasticuser.type==1)result=autenticate(afantasticuser);
   else{if (afantasticuser.type==2)result=registeru(afantasticuser);
	else perror("Malformetted data from client(security allert: attack with a row socket is plausible )");
         exit(1); }

    //write result to the stream
    if (write (c,&result, sizeof (int)) < 0) {
	perror ("read");
	exit (1);

	 }
    tentatives++;
    if (tentatives>=5){
       //close stream if client cant log in in five times kill also the child process 
       shutdown(c,2);
       close(c);
       exit(1);
    }
    }while(result!=1);
    
    //from now we are logged so the server can start recive varius type of packages
    //lets check if we have alredy a vdr for this client and if not select one randomicaly;
    checkvdr(afantasticuser,vdrs);

    do{
        if (read (c, &type, sizeof (int)) < 0) {
	perror ("read");
	exit (1);
	 }
	 switch (type) {
	 	case 7: //if the type is 7 client want the number of inbox message  ;
		    
	 		
	 		break;
	 	default:
	 		
	 }


    }while(type!=5)//type 5 close the connection with the client 


    shutdown (c,2);
    close (c);	    
}

void main()
{
    int s,c,len,vdr,vdrs[VDRN];//vdr is for the master socket of VDRS while on aceptance you have filled the array vdrs[VDRN],s is for the master socket for client while on aceptance you have the socket c 
    struct sockaddr_in saddr;
    int ops[3];
    int addr;
    int running;//number of running client
    int maxspawn;//max number of  running client as far as Mds process is started;
    int runningvdr;//number of  vdr actualy connected 
    //create the stream socket for the VDRs 
    if ((vdr=socket(AF_INET,SOCK_STREAM,0))<0) {
	perror("socket");
	exit(1);
    }
    puts("client socket done");

    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(16000);
    saddr.sin_addr.s_addr=INADDR_ANY;

    if (bind(vdr,(struct sockaddr *)&saddr,sizeof(saddr))){
	perror("bind");
	exit(1);
    }
    puts("bind done");

    if (listen(vdr,5)) {
	perror("listen");
	exit(1);
    }
    puts("listen done");

    while(runningvdr<=VDRN){
      len = sizeof (saddr);
      if ((vdrs[runningvdr] = accept (vdr, (struct sockaddr *) &saddr, &len)) < 0) {
	    perror ("accept");
	    exit (1);
	}
      addr = *(int *) &saddr.sin_addr;
      printf ("accept - connection from vdr %d.%d.%d.%d\n",
		(addr & 0xFF), (addr & 0xFF00) >> 8, (addr & 0xFF0000) >> 16,
		(addr & 0xFF000000) >> 24);
	//type for the echo call to a vdr process
      int echo=1;
	//write the echo in the socket 
      if (write(vdrs[runningvdr],&echo,sizeof(echo))<0) {
	perror("write");
	exit(1);
	}
      wait(100);//time to get the result write in the socket by vdr
      //read from the socket the value 
      if (read(vdr[runningvdr],&echo,sizeof(echo))<0) {
	perror("write");
	exit(1);}

      //ceck if the response is valid and if its not close the connection 
      if(echo!=1){
	   close(vdrs[runningvdr]); }else runningvdr++;
     
    }


    //create the stream socket for the client 
    if ((s=socket(AF_INET,SOCK_STREAM,0))<0) {
	perror("socket");
	exit(1);
    }
    puts("client socket done");

    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(20000);
    saddr.sin_addr.s_addr=INADDR_ANY;

    if (bind(s,(struct sockaddr *)&saddr,sizeof(saddr))){
	perror("bind");
	exit(1);
    }
    puts("bind done");

    if (listen(s,5)) {
	perror("listen");
	exit(1);
    }
    puts("listen done");
    
    running=0;
    maxspawn=0;
    
    while (1) {
	len = sizeof (saddr);
	if ((c = accept (s, (struct sockaddr *) &saddr, &len)) < 0) {
	    perror ("accept");
	    exit (1);
	}
	addr = *(int *) &saddr.sin_addr;
	printf ("accept - connection from  %d.%d.%d.%d\n",
		(addr & 0xFF), (addr & 0xFF00) >> 8, (addr & 0xFF0000) >> 16,
		(addr & 0xFF000000) >> 24);
	switch (fork()) {
	    case 0:
		close(s);
		dowork(c,vdrs);
	    	exit(0);
	    case -1:
	    	perror("fork");
	    	break;
	    default:
	    	close (c);	    
	    	running +=1;
		if (running > maxspawn) {
		    maxspawn=running;
		    printf ("== Max:%d\n",maxspawn);
		}
		while (waitpid(-1,NULL, WNOHANG)>0) running -=1;
	    	break;
	}

    }
    close(s);
    close(vdr)
    for(i=0;i<=VDRN;i++){close(vdrs[i]);}   

}
