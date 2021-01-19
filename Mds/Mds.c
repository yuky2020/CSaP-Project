#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include "util.h"
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#define SEM_NAME "/semaphore"//is the prefix in the name of every semaphore used for VDR
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)// "Permessi del semaforo"
#define INITIAL_VALUE 1//intial value of the semaphore 
#define VDRN 1 //Number of vdr used
//get the number of inbox message of a logged user;
int giveInbox(userData afantasticuser;int vdrIndex, int vdrs[VDRN]){
   sem_t *sem;
   int vdrtype=7 //7 is the type to write in the socket for this call
   int inboxvdr=0 //the buffer for the returning value    
   char saddress[(MAXLIMIT+10)] = {'\0'};//the adress of the semphore for the i vdr 
   sprintf(saddress, "%s.%d", SEM_NAME,vdrIndex);
   sem= sem_open(saddress, O_RDWR);//open the semaphore 
   if (sem == SEM_FAILED) {
      perror("sem_open failed");
      exit(EXIT_FAILURE);
      }
   sem_wait(sem);//lock the semaphore so you can write to the VDR socket without risk 
   //write the type of request to the socket
   //write vdrtype to the socket    
   if (write(vdrs[i],&vdrtype,sizeof(vdrtype))<0) {
	 perror("write");
	 exit(1);}
   //write username to the socket
   if (write(vdrs[i],&afantasticuser.username,sizeof(afantasticuser.username))<0) {
	 perror("write");
	 exit(1);}
   wait(100);//give time to recive data and send the response from the i-th vdr process
   //read the result from actual vdr
	    if (read(s,&inboxvdr,sizeof(inboxvdr))<0) {
	    perror("read");
	    exit(1);}

   sem_post(sem);//unlock the semaphore "incresing the  counter"
   if (sem_close(sem) < 0){
      perror("sem_close(3) failed");
      exit(0);}
   return inboxvdr;



}



//get the vdr of a logged user
//naturaly you shuold always wait to check if other process is using the semaphore for vdr socket
int getvdrIndex(userData afantasticuser;int vdrs[VDRN] ){
   int    retvdr=0;//the return value from the vdr set to default to 0; 
   int    vdrtype=9; //the type to send to the vdr in order to get back if the user is or not in it;
   for(i=0;i<VDRN;i++){
	    sem_t *sem;
	    char saddress[(MAXLIMIT+10)] = {'\0'};//the adress of the semphore for the i vdr 
	    sprintf(saddress, "%s.%d", SEM_NAME,i);
            sem= sem_open(saddress, O_RDWR);//open the semaphore 
            if (sem == SEM_FAILED) {
            perror("sem_open failed");
            exit(EXIT_FAILURE);
            }
            sem_wait(sem);//lock the semaphore so you can write to the VDR socket
	    //write vdrtype to the socket    
	    if (write(vdrs[i],&vdrtype,sizeof(vdrtype))<0) {
		  perror("write");
		  exit(1);}
	    //write username to the socket
	    if (write(vdrs[i],&afantasticuser.username,sizeof(afantasticuser.username))<0) {
		  perror("write");
		  exit(1);}
	    wait(100);//give time to recive data and send the response from the i-th vdr process
            //read the result from actual vdr
	    if (read(s,&retvdr,sizeof(retvdr))<0) {
	    perror("read");
	    exit(1);}

            sem_post(sem);//unlock the semaphore "incresing counter"
	    if (sem_close(sem) < 0){
            perror("sem_close(3) failed");
            exit(0);}
	    //if retvdr==1 the user data is in that vdr so we can return here 
            if(retvdr==1)return i;           

	    } 
    return 0; //if no vdr contains information return 0 you will use the frist;
   

}


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



//we do all the work with a single client in this function,is used by a child process
//parameter are the int c that identify  the scoket for the client and vdrs[VDRN] that are a list of socket vdr
void dowork(int c,int vdrs[VDRN])
{
    //@var User to login or register 
    userData afantasticuser;
    int type,result,tentatives=0;
    int vdrIndex;//the index of vdr to use
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
	perror ("write");
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
    vdrIndex=getvdrIndex(afantasticuser,vdrs);

    do{
        if (read (c, &type, sizeof (int)) < 0) {
	perror ("read");
	exit (1);
	 }
	 switch (type) {
	 	case 7: //if the type is 7 client want the number of inbox message  ;
		   {int inbox=giveInbox(afantasticuser,vdrIndex,vdrs);//this function return the number of inbox Audio message of a user;
		     if (write (c,&inbox, sizeof (int)) < 0) {
			perror ("write");
			exit (1);

			}

		   } 
	 		
	 		break;
	 	default:
	 		
	 }


    }while(type!=5)//type 5 close the connection with the client 


    shutdown (c,2);
    close (c);	    
}

void main()
{
      
    sem_t *semvdr[VDRN];//semphore to prevent two client from write to the same vdr using poisix semphore because more easy to menage
    int s,c,len,vdr,vdrs[VDRN];//vdr is for the master socket of VDRS while on aceptance you have filled the array vdrs[VDRN],s is for the master socket for client while on aceptance you have the socket c 
    struct sockaddr_in saddr;
    int ops[3];
    int addr;
    int running;//number of running client
    int maxspawn;//max number of  running client as far as Mds process is started;
    int runningvdr;//number of  vdr actualy connected
    //create semaphores for vdr
    for(int j=0;j<VDRN;j++){
    char saddress[(MAXLIMIT+10)] = {'\0'};//the adress of the semphore for the j vdr 
    sprintf(saddress, "%s.%d", SEM_NAME,j);

    // create, initialize semaphore
    semvdr[j] = sem_open(saddress, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);
    if (semvdr[j] == SEM_FAILED) {
        perror("sem_open error");
        exit(EXIT_FAILURE);
    }
    
    }    
    

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
    // close  client "Master socket"
    close(s);
    //close vdr "Master socket"
    close(vdr)
    //close each vdr socket   
    for(i=0;i<=VDRN;i++){close(vdrs[i]);} 
    //unlink and destroy vdr named semaphore
    for(int j=0;j<VDRN;j++){
    char saddress[(MAXLIMIT+10)] = {'\0'};//the adress of the semphore for the j vdr 
    sprintf(address, "%s.%d", SEM_NAME,j);

    if (sem_unlink(saddress) < 0) //unlink the semaphore
    perror("sem_unlink failed");   
    sem_destroy(semvdr[j]);//destroy the semaphore after usage good pratice



}
