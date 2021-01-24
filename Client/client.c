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
int serchMessages(char user[MAXLIMIT,int s]){
  int mdsRet;//return value from mds;
  int type=8;//the type of call for get all messages destinated to a user;
  int inboxn=checkinbox(s)//recall now because inbox number could change in small time 
  //PackageData *c=malloc(1*sizeof(PackageData));
  PackageData messageList[inboxn];
    //wirte in the socket data for login or register
do{  if (write(s,&type,sizeof(type))<0) {
	perror("write");
	return 1;
     }
    printf("##############");
    printf("Serch for  messages\n");
    printf("##############");
    printf("");//to implement

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
    int i;    
    for(i=(inboxn-1);i>=0;i--){//print like that for have the last message frist
      printf("%d)Message from %s sended %s\n",(inbox-i),messageList[i].from,messageList[i].timestamp);
  
      }

    scanf("%d",&i);
    if(i>=ninbox){printf("error message not found 404"); wait(1000); i=3;}//set a new value for i so you can back to all message;
    else{
      while(i<3){//if 3 the function reload all the message and you can list it again;
      //playback the message
      playback(messageList[i].message);      
      printf("From %s \nTime:%s\n",messageList[i].from,messageList[i].timestamp);
      printf("1) for listen again \n");
      printf("2)to inoltrate the message\n");
      printf("3)come back to inbox \n");
      printf("4)go back to main page\n");
      scanf("%d",&i);
     if(i==2){messageList[i].to=selectuserto(s); //inoltrate to a new user the message;
	      messageList[i].from=username;
             if(sendMessage(messageList[i],s))printf("MESSAGE NOT SENT NETWORK PROBLEM TRY AGAIN \n");//sendMessage return 1 upon fail
	      else{printf("MESSAGE SENT SUCESSFULLY\n");}
		  

	      }
          

      
  }
    }while(i!=4);
free(messageList);//good thing to do
return 0;//return 0 in case of success;
  
   

  






}
//add one user to the adressBook after check it it is or not enroled in the server 
int addusertoadressbook(int s){
  FILE *fp;//file where to place adressBook;
  char usernameList[ADDRESSBOOKLIMIT][MAXLIMIT];
  char toadd[MAXLIMIT];//username to add to the adressBook;
  int j,trov,i=0;//for save the number of user and for going trought it 
  if ((fp = fopen("AdressBook", "rb")) == NULL) {
      perror(" opening file");
      // Program exits if file pointer returns NULL.
      return 1;
    }
  int j=0;

  while (!feof(fp)){// while not end of file
           fread(usernameList[j],sizeof(char[MAXLIMIT]),1,fp);
	   j++;
            
      }
  j--;//becouse you encrement before re enter
  fclose(fp);//close the file for now

  printf("insert username to add\n");
  scanf("%s",toadd);
  //check if the user is alredy in the adressBook;
  for (i=0;i<=j;i++){
  if (strcmp(usernameList[i],toadd)==0){
  printf("This user is alredy in the list");
  return 1;}
  
  }
  //check if the user exist() from server
  int type=9;//the type of call for this function
  if (write(s,&type,sizeof(type))<0) {
	perror("write");
	return 1;}
   	
  if (read(s,&j,sizeof(int))<0) {
	perror("write");
	return 1;}
  if(j<1){perror("problem with MDS"); return 1;}
  int usernameList[j][MAXLIMIT];
  trov=0;//set trov =1 when we find a matching 

  //read all username and print it 
  for(int k=0;k<j;j++){
    //read from socket and print for the user
    if (read(s,&usernameList[k],sizeof(char[MAXLIMIT]))<0) {
	perror("write");
	return 1;}
    if(strcmp(usernameList[k],toadd)=0)trov=1;
  }
  if (read(s,&type,sizeof(int)<0) {
	perror("write");
        return 1;}
  //type is used to store the end of trasmission from MDS;	
  if(type!=1) {
	perror("write");
        return 1;}
  type=5;//end of trasmission for MDS
  if (write(s,&type,sizeof(type))<0) {
	perror("write");
	return 1;}
  //if i found it i can add it to the adressBook
  if(trov){
	    if ((fp = fopen("AdressBook", "ab")) == NULL) {//open finaly in append mode to add the new user
		perror(" opening file");
		// Program exits if file pointer returns NULL.
		return 1;
		      }
	    fwrite(toadd,sizeof(char[MAXLIMIT]),1,fp);
	    fclose(fp);
	    return 1;

	}
  return 0;



 
}    
const char * selectuserto(int s){
  int j,i=0;//j is the nuber of user actualy in server
  printf("1)search from adressBook\n");
  printf("2)search from all user\n");
  scanf("%d",&i);
  printf("select wanted user by number");
  if(i==1){
      FILE *fp;//file where to place adressBook;
      char usernameList[ADDRESSBOOKLIMIT][MAXLIMIT];//list from adressBook


      if ((fp = fopen("AdressBook", "rb")) == NULL) {
	    perror(" opening file");
	    // Program exits if file pointer returns NULL.
	    return 1;
	    }
      j=0;

      while (!feof(fp)){// while not end of file
	    fread(usernameList[j],sizeof(char[MAXLIMIT]),1,fp);
	    printf("%d,%s\n",(j+1),usernameList[j]);
	    j++;
		}
      j--;//becouse you encrement before re enter
      fclose(fp);//close the file for now
      int k;
      do{scanf(%d,&k);
	 if(k>j)printf("this number is still not presnt");
	}while(k>j);
      char tmp[MAXLIMIT];
      strcpy(tmp,userList[i]);
      free(usernameList[i]);//fre the user list not useful anymore;
      return tmp;

      }

  if(i==2){//retrive all user and selsct from the list 
    int type=9;//the type of call for this function
    if (write(s,&type,sizeof(type))<0) {
	perror("write");
	return 1;}
   	
    if (read(s,&j,sizeof(int))<0) {
	perror("write");
	return 1;}
    if(j<1){perror("problem with MDS"); return 1;}
    int usernameList[j][MAXLIMIT];

    //read all username and print it 
    for(int k=0;k<j;j++){
      //read from socket and print for the user
      if (read(s,&usernameList[k],sizeof(char[MAXLIMIT]))<0) {
	  perror("write");
	  return 1;}
      printf("%d,%s\n",(k+1),usernameList[k]);
      
      }
    if (read(s,&type,sizeof(int)<0) {
	  perror("write");
	  return 1;}
    //type is used to store the end of trasmission from MDS;	
    if(type!=1) {
	  perror("write");
	  return 1;}
    type=5;//end of trasmission for MDS
    if (write(s,&type,sizeof(type))<0) {
	  perror("write");
	  return 1;}
	

    //actual_user enter the user wanted;
    do{ scanf("%d",&i);
      if (i>j)printf("This user is still not in list try again \n");
      }while(i>j);

    char tmp[MAXLIMIT];
    strcpy(tmp,usernameList[i]);
    free(usernameList[i]);//fre the user list not useful anymore;
    return tmp;
    }



}

//list all mesage sended to this user contact the MDS to get data, is an interactive function 
int listallmessage(int s,char username[MAXLIMIT]){
  int mdsRet;//return value from mds;
  int type=8;//the type of call for get all messages destinated to a user;
  int inboxn=checkinbox(s)//recall now because inbox number could change in small time 
  //PackageData *c=malloc(1*sizeof(PackageData));
  PackageData messageList[inboxn];
    //wirte in the socket data for login or register
do{  if (write(s,&type,sizeof(type))<0) {
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
    int i;    
    for(i=(inboxn-1);i>=0;i--){//print like that for have the last message frist
      printf("%d)Message from %s sended %s\n",(inbox-i),messageList[i].from,messageList[i].timestamp);
  
      }

    scanf("%d",&i);
    if(i>=ninbox){printf("error message not found 404"); wait(1000); i=3;}//set a new value for i so you can back to all message;
    else{
      while(i<3){//if 3 the function reload all the message and you can list it again;
      //playback the message
      playback(messageList[i].message);      
      printf("From %s \nTime:%s\n",messageList[i].from,messageList[i].timestamp);
      printf("1) for listen again \n");
      printf("2)to inoltrate the message\n");
      printf("3)come back to inbox \n");
      printf("4)go back to main page\n");
      scanf("%d",&i);
     if(i==2){messageList[i].to=selectuserto(s); //inoltrate to a new user the message;
	      messageList[i].from=username;
             if(sendMessage(messageList[i],s))printf("MESSAGE NOT SENT NETWORK PROBLEM TRY AGAIN \n");//sendMessage return 1 upon fail
	      else{printf("MESSAGE SENT SUCESSFULLY\n");}
		  

	      }
          

      
  }
    }while(i!=4);
free(messageList);//good thing to do
return 0;//return 0 in case of success;
  
   

  


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
  printf("4) search messages\n");
  ptintf("9)EXIT\n");
  scanf("%d",&i);
  switch (i) {
    case 1:{
	   listallmessage(s,afantasticuser.username);
	 }
    case 2 ://select a user and then send a message user can be select eihter by address book or by write is name ;
	 
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
    case 3:{
	     if(addusertoadressbook(s)){perror("Error,Try Again")};
	   }
  
    case 4:{
	     serchMessages(s,afantasticuser.username);
	   }	   
    	
        
    default: printf("This option is not avaible \n"); 
        
    }
  }while(i!=9);
}
