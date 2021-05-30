#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include "../lib/util.h"

//check if a user has ever logged here
int checkuser(char user[MAXLIMIT]){
   FILE *fp;
   char address[(MAXLIMIT+11)] = {'\0'};

   sprintf(address,"users/%sInbox",user);
   //check if the file  exist and if it is return 1 else return 0;
   if ((fp = fopen(address, "rb")))return 1;
   return 0;
}   
//get number of message in inbox for a user ;
int getinbox(char user[MAXLIMIT]){
   int ninbox=0;
   FILE *fp;
   // save the address of the file the name is the usename of the reciver 
   char address[(MAXLIMIT+11)] = {'\0'};

   sprintf(address,"users/%sInbox",user);
   //check if the file  exist and if it is read the number of inbox then close the file
   if ((fp = fopen(address, "rb")))
    {
     if (fread(&ninbox,sizeof(ninbox),1,fp)<0) {
	perror("read");
	return 0;
	}
   fclose(fp);}

   return ninbox;
}
//function to return the message stored in a file by is username 

int readandsendMessages(char user[MAXLIMIT],int s ){
   int closetype=5;
   int i= getinbox(user);
   FILE *fp;
   PackageData tmp;//package data where to store data while reading and before check
   // save the address of the file the name is the usename of the reciver 
   char address[(MAXLIMIT+11)] = {'\0'};
   sprintf(address, "users/%sData", user);
    //open in read mode    
    fp = fopen(address,"rb");
    if(fp == NULL){perror("error open");
		    return 1;}
    //Start to reading and sending all PackageData destinated to  user ones to ones;also deserialize it 
    for(i;i>=1;i--){
        //read the type from file 
        if (fread(&tmp.type,sizeof(tmp.type),1,fp)<0) { 
	perror("read");
	return 0;
	}
        //read the from user from file
        if (fread(&tmp.from,sizeof(tmp.from),1,fp)<0) { 
	perror("read");
	return 0;
	}
	//read the to user from file 
        if (fread(&tmp.to,sizeof(tmp.to),1,fp)<0) { 
	perror("read");
	return 0;
	}
	//read the size of AudioDataf
        if (fread(&tmp.size,sizeof(tmp.size),1,fp)<0) { 
	perror("read");
	return 0;
	}
	//read audioDataf
        if (fread(&tmp.message,tmp.size,1,fp)<0) { 
	perror("read");
	return 0;
	}
	//read the hash
        if (fread(&tmp.hash,sizeof(tmp.hash),1,fp)<0) { 
	perror("read");
	return 0;
	}
	//read the timestamp
        if (fread(&tmp.timestamp,sizeof(tmp.timestamp),1,fp)<0) { 
	perror("read");
	return 0;
	}
	//check if the hash is still valid
        if(tmp.hash!=hashCode(tmp)){
	perror("hash is different");
	return 0;}

	//now send the data trought socket and reserialize it
        if (write(s,&tmp.type,sizeof(tmp.type))<0) {
	perror("write");
	return 0;}
        
        if (write(s,&tmp.from,sizeof(tmp.from))<0) {
	perror("write");
	return 0;}
        
        if (write(s,&tmp.to,sizeof(tmp.to))<0) {
	perror("write");
	return 0;}
        

        if (write(s,&tmp.size,sizeof(tmp.size))<0) {
	perror("write");
	return 0;}
        

       if (write(s,&tmp.message,tmp.size)<0) {
	perror("write");
	return 0;}
       
       
       if (write(s,&tmp.hash,sizeof(tmp.hash))<0) {
	perror("write");
	return 0;}
       
 
       if (write(s,&tmp.timestamp,sizeof(tmp.timestamp))<0) {
	perror("write");
	return 0;}
       
       }
   //close the file because we have done with it 
   fclose(fp);
   //write the closetype inide the socket so Mds know that message sending is over   //write the closetype inide the socket so Mds now that message sending is over   //write the closetype inide the socket so Mds now that message sending is over   //write the closetype inide the socket so Mds now that message sending is over   //write the closetype inide the socket so Mds now that message sending is over   //write the closetype inide the socket so Mds now that message sending is over   //write the closetype inide the socket so Mds now that message sending is over
   if (write(s,&closetype,sizeof(closetype))<0) {
    perror("write the closetype");
    return 0;}
   //if evrything goes fine return 1
   return 1;
      
}

//function to store a message to a file and add 1 to ninbox 
int storetofile(PackageData tostore){
    int ninbox=0;
    FILE *fp;
    // save the address of the file the name is the usename of the reciver 
   char address[(MAXLIMIT+11)] = {'\0'};

   sprintf(address, "users/%sData", tostore.to);
    //open in append mode    
    fp = fopen(address,"ab");
    if(fp == NULL){perror("error open");
		    return 1;}
        //write the PackageData to the file and serialize it 
        if (fwrite(&tostore.type,sizeof(tostore.type),1,fp)<0) { 
	perror("write");
	return 1;
	}
        //write the from user to file
        if (fwrite(&tostore.from,sizeof(tostore.from),1,fp)<0) { 
	perror("write");
	return 1;
	}
	//write the to user to file 
        if (fwrite(&tostore.to,sizeof(tostore.to),1,fp)<0) { 
	perror("write");
	return 1;
	}
	//write the size of AudioDataf
        if (fwrite(&tostore.size,sizeof(tostore.size),1,fp)<0) { 
	perror("write");
	return 1;
	}
	//write audioDataf
        if (fwrite(&tostore.message,tostore.size,1,fp)<0) { 
	perror("write");
	return 1;
	}
	//write the hash

        if (fwrite(&tostore.hash,sizeof(tostore.hash),1,fp)<0) { 
	perror("write");
	return 1;
	}
	//write the timestamp
        if (fwrite(&tostore.timestamp,sizeof(tostore.timestamp),1,fp)<0) { 
	perror("write");
	return 1;
	}

    fclose(fp);
    sprintf(address,"users/%sInbox", tostore.to);
    //check if the file alredy exist and if it is read the number of inbox then close the file
    printf(" a new message inserted ");
    if ((fp = fopen(address, "rb")))
    {
     if (fread(&ninbox,sizeof(int),1,fp)<0) {
	perror("read");
	return 1;
	}
     fclose(fp);
    }    
    //increment the number of inbox
    ninbox++;
    //now open and  write on the file the nof inbox 
    fp = fopen(address,"wb");
    if(fp == NULL){perror("error opening"); return 1;}
    
    if (fwrite(&ninbox,sizeof(int),1,fp)<0) {
	perror("write");
	return 1;
	}
    //finaly return 0 if evrything goes fine 
    return 0;
}





//function to store a Message from MDS
int storeMessage(int s){
       
        PackageData tostore;
        if(recive_PackageData(&tostore,s)){
                perror("Error in reciving data");
                return 1;}

        //finaly store the data
        if(storetofile(tostore)){
	        perror("store");
	        return 1;}
    //then return 0 if everything goes fine 
    return 0;
}

//delate a message alredy store for a user
int delateMessage(int s){
 PackageData todel;//package to delate
 int 	     trov=0;//variabile to set if i have found and delate the file
    todel.type=6;
    //read the user from the data is sended 
    if (read(s,&todel.from,sizeof(todel.from))<0) {
	perror("read");
	return 1;}
    //read the user to the message is sended
    if (read(s,&todel.to,sizeof(todel.to))<0) {
	perror("read");
	return 1;}
    //read the size of audio data
    if (read(s,&todel.size,sizeof(todel.size))<0) {
	perror("read");
	return 1;}
    //read the audioData 
    if (read(s,&todel.message,todel.size)<0) {
	perror("read");
	return 1;}

    //read the hash
    if (read(s,&todel.hash,sizeof(todel.hash))<0) {
	perror("read");
	return 1;}
    //read timestamp
    if (read(s,&todel.timestamp,sizeof(todel.timestamp))<0) {
	perror("read");
	return 1;}
    //check if the hash is still valid 
    if(todel.hash!=hashCode(todel)){
	perror("hash is different");
	return 1;}

   //inizializethe read of all the package for this user;
   //read number of message for this user 
   int j= getinbox(todel.to);
   int i=0;
   FILE *fp;
   PackageData tmp[i];//package data where to store data while reading and before check
   // save the address of the file the name is the usename of the reciver 
   char address[(MAXLIMIT+11)] = {'\0'};
   sprintf(address, "users/%sData", todel.to);
    //open in read mode    
    fp = fopen(address,"rb");
    if(fp == NULL){perror("error open");
		    return 1;}
    //Start to reading and sending all PackageData destinated to  user ones to ones;also deserialize it 
    for(i;i<j;i++){
        //read the type from file 
        if (fread(&tmp[i].type,sizeof(tmp[i].type),1,fp)<0) { 
	perror("read");
	return 0;
	}
        //read the from user from file
        if (fread(&tmp[i].from,sizeof(tmp[i].from),1,fp)<0) { 
	perror("read");
	return 0;
	}
	//read the to user from file 
        if (fread(&tmp[i].to,sizeof(tmp[i].to),1,fp)<0) { 
	perror("read");
	return 0;
	}
	//read the size of AudioDataf
        if (fread(&tmp[i].size,sizeof(tmp[i].size),1,fp)<0) { 
	perror("read");
	return 0;
	}
	//read audioDataf
        if (fread(&tmp[i].message,tmp[i].size,1,fp)<0) { 
	perror("read");
	return 0;
	}
	//read the hash
        if (fread(&tmp[i].hash,sizeof(tmp[i].hash),1,fp)<0) { 
	perror("read");
	return 0;
	}
	//read the timestamp
        if (fread(&tmp[i].timestamp,sizeof(tmp[i].timestamp),1,fp)<0) { 
	perror("read");
	return 0;
	}
	//check if the hash is still valid
        if(tmp[i].hash!=hashCode(tmp[i])){
	perror("hash is different");
	return 0;}
      }
    //now i have to all the data stored and can delate the file_data
   fclose(fp);
   //destroy the file 
   fclose(fopen(address, "wb"));
   for(i=0;i<j;i++){
      if(strcmp(todel.timestamp,tmp[i].timestamp)==0&&todel.hash==tmp[i].hash)trov=1;
      else{ //store the data
	 if(storetofile(todel)){
	    perror("store");
	    return 1;}


      }
   }
   //we have done with the file 
    fclose(fp);
    //at this point we have recreated the file without the todel PackageData
    //check if it was in the old data 
    if(trov==0)return 1;
    //if evrything is done return 0
    return 0;


}
int  main(int argc, char *argv[])
{
    int s,socret; //s is for the socket ,socret is for returning some information to the socket 
    struct sockaddr_in saddr;
    struct hostent *hst;
    char hostname[MAXLIMIT];
    char port[5];   
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
    if ((hst=gethostbyname("localhost"))==NULL) {
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
    
    // read the type of call from socket
    if (receive_int(&type,s)<0) {
	perror("write");
	exit(1);
    }
    //type 1 for echo
    if(type==1){
    // Read (or recv) from socket
    printf("echo call from Mds \n");
    //return the same value 1 for notifing that the Vdr is online ;
    if (send_int(type,s)<0) {
	perror("write");
	exit(1);
	}}
    //type six for message to store
    if(type==6){ 
	//set socret=1 for returning it to the MDS
	socret=1;
	//if the file is not stored set socret=0
	if(storeMessage(s))socret=0;
	//write socret in the socket
        if (send_int(socret,s)<0) {
	        perror("send");
	        exit(1);}
        }
    //type seven for get ninbox
    if(type==7){
	char user[MAXLIMIT];
        int len;
        //read the len of the user you are about to recive
         if (receive_int(&len,s)<0) {
	perror("read");
	exit(1);}
	//read the username to search inbox;
        if (read(s,user,len+1)<0) {
	perror("read");
	exit(1);}
        //socret= ninbox is 0 either if the file with ninbox dosen't esist or if ninbox=0;
	socret=getinbox(user);
        //return socret to the socket
        if (send_int(socret,s)<0) {
	perror("write");
	exit(1);}
       }
   //type 8 for get all the message from a user sending it one to one to avoid huge package  
    if(type==8){
      	char user[MAXLIMIT];
	//read the username to search Audiodata;
        if (read(s,&user,sizeof(user))<0) {
	perror("read");
	exit(1);}
       //return 0 if there is an error and 1 if everything has gone fine       //return 0 if there is an error and 1 if everything has gone fine  
       if(!readandsendMessages(user,s)){perror("cant send message"); exit(1);}

	}
    //type 9 is for check if the user was ever register in this vdr;
    if(type==9){
	char user[MAXLIMIT];
        int len;
	//read the username to search inbox;
        if (receive_int(&len,s)<0) {
	perror("read");
	exit(1);}
        if(read(s,user,len+1)<0){
                perror("read");
                exit(1);
        }
        printf("Im chek if %s is here \n",user );
        //socret= return 0 if the file with ninbox dosen't esist 1 else ;
	socret=checkuser(user);
        //return socret to the socket
        if (send_int(socret,s)<0) {
	perror("write");
	exit(1);}
    }
   //type 10 to delate a message;	
    if(type==10){
	//set socret=1 for returning it to the MDS
	socret=1;
	//if the file is not stored set socret=0
	if(delateMessage(s))socret=0;
	//write socret in the socket
        if (write(s,&socret,sizeof(socret))<0) {
	perror("write");
	exit(1);}
    }

     
    

    
    }while(type!=5);
        // Close the client socket
    close(s);

}

