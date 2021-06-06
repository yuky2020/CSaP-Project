#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include"../lib/util.h"
//function to select a user form addressbook or from all user @param s:socket to MDS,@param *tmp string  to save the user to;
int  selectuserto(int s,char *tmp){
  int len; //lenght of evry char that is sended;
  int j,ru,i=0;//ru is the nuber of user actualy in server
  printf("1)search from adressBook\n");
  printf("2)search from all user\n");
  scanf("%d",&i);
  printf("select wanted user by number\n");
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
      do{scanf("%d",&k);
	 if(k>j)printf("this number is still not presnt");
	}while(k>j);
      strcpy(tmp,usernameList[(k+1)]);
    //  free(usernameList);//fre the user list not useful anymore;
      return 1;

      }

  else if(i==2){//retrive all user and selsct from the list 
      int type=9;//the type of call for this function
      if (send_int(type,s)) {
	        perror("write");
	        return 1;}
      sleep(1);
      printf("Sto per ricevere dei dati \n");	
      if (receive_int(&ru,s)) {
	        perror("write");
	        return 1;
          }
      //at the lest there is one user
      printf("actualy registred users %d \n",ru);
      sleep(5); 
      if(ru<=1){perror("problem with MDS"); return 1;}
      char usernamesist[ru][MAXLIMIT];
      sleep(1);
      //char usernameList[MAXLIMIT][MAXLIMIT]; here for testing pourpose
      //read all the username and print it 
      for(int k=0;k<ru;k++){
        //read the lenght of the next string; 
        if(read(s,&len,sizeof(int))<0){perror("read"); return 1; }
        //read from socket and print for the user
        if (read(s,usernamesist[k],len+1)<0) {
	        perror("read");
	        return 1;}
          usernamesist[k][len] = '\0';  /* Terminate the string! */  
       printf("%d  %s\n",(k+1),usernamesist[k]);
        
      
        }
    if (receive_int(&type,s)<0) {
	  perror("read");
	  return 1;}
    //type is used to store the end of trasmission from MDS;	
    if(type!=1) {
	  perror("type");
	  return 1;}
    type=5;//end of trasmission for MDS
    if (send_int(type,s)<0) {
	  perror("write");
	  return 1;}
	
    printf("select wanted user ");
    //actual_user enter the user wanted;
    do{ scanf("%d",&i);
        i--;
      if (i>ru||i<0)printf("This user is still not in list try again \n");
      }while(i>ru||i<0);

    strcpy(tmp,usernamesist[i]);
    //free(usernameList);//fre the user list not useful anymore;
   
    }


 return 0;
}

//return number of massage stored actualy in Inbox 
int checkinbox(int s){
  int ret,type=7; //7 is the type for this call
    //write type in the socket
    if (send_int(type,s)<0) {
	    perror("write");
	    exit(1);
      }
    //then the socket should return the number of massage
    if (receive_int(&ret,s)<0) {
	    perror("read");
	    exit(1); }
     return ret;
}

//write in  the socket type=5,so Mds whill close child process;
int disconnect(int s){
  int type=5;
  if (send_int(type,s)<0) {
	  perror("write");
	  return 0;}
  return 1;

}
//Function to send a message to MDS @Param PackageData:Struct that contains the message,and other value see util.h @Param s socket conected  to MDS

//function to tell mds to delate a message 
int delateMessage(PackageData todel,int s){
  int type=10; // type for this call;
    //wirte the type in the socket
  if (write(s,&type,sizeof(type))<0) {
	perror("write");
	return 1;
      }

  if(send_PackageData(todel,s)){perror("send Message"); return 1;}
  //check the return value from MDS;
  if (read(s,&type,sizeof(type))<0) {
	perror("read");
	return 1;}
  if(type)return 1;
  else return 0;
  }

//function to search for a message in the inbox ;//download the inbox is not the best way, but make possible to do search more time without recall MDS //same method used by some  email client 
int serchMessages(char user[MAXLIMIT],int s){
  char toserchu[MAXLIMIT];//the string to search for a user  ;
  char toserchd[2][TIMESTAMPS];//the date between search
  int mdsRet;//return value from mds;
  int k,i,t=0;//for select among user or date serch and valid or not ;
  int type=8;//the type of call for get all messages destinated to a user;
  int inboxn=checkinbox(s);//recall now because inbox number could change in small time 
  //PackageData *c=malloc(1*sizeof(PackageData));
  PackageData messageList[inboxn];
    //wirte the type in the socket
do{  if (write(s,&type,sizeof(type))<0) {
	perror("write");
	return 1;
     }
    printf("##############\n");
    printf("Serch for  messages\n");
    printf("##############\n");
    printf("1)search for a user messages\n");
    printf("2) serch for messages in a date range\n");
    printf("3)serch for a user messages in a date range\n");
    scanf("%d",&k);
    if(k==1||k==3) selectuserto(s,toserchu); 
    if(k==2||k==3){printf("write the start date to search in format:es Sat Mar 25 06:10:10 1989  \n");
		   scanf("%s",toserchd[0]);
		   printf("write the final date to search \n");
		   scanf("%s",toserchd[1]);}


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
      t=0;
      if(k==1||k==3)if(strcmp(messageList[i].from,toserchu)==0)t=1;//check fot the user 
      if(k==2||k==3){ if(datecmp(messageList[i].timestamp,toserchd[0])==1&&datecmp(toserchd[1],messageList[i].timestamp)) 
                          t=1;//check for the date 
		                    else t=0;
                        } 
     if(t==1)printf("%d)Message from %s sended %s\n",(inboxn-i),messageList[i].from,messageList[i].timestamp);
  
      }
    scanf("%d",&i);
    if(i>=inboxn){printf("error message not found 404"); sleep(1); i=4;}//set a new value for i so you can back to all message;
    else if(i!=0){
      while(i<4){//if 3 the function reload all the message and you can list it again;
      //playback the message
      i=(inboxn-i);	
      playback(messageList[i].message);      
      printf("From %s \nTime:%s\n",messageList[i].from,messageList[i].timestamp);
      printf("1) for listen again \n");
      printf("2)to inoltrate the message\n");
      printf("3)to delete the message\n");
      printf("4)come back to inbox \n");
      printf("5)go back to main page\n");
      scanf("%d",&i);
     if(i==2){selectuserto(s,messageList[i].to); //inoltrate to a new user the message;
	      strcpy(messageList[i].from,user);
             if(send_PackageData(messageList[i],s))printf("MESSAGE NOT SENT NETWORK PROBLEM TRY AGAIN \n");//sendMessage return 1 upon fail
	            else{printf("MESSAGE SENT SUCESSFULLY\n");
	              i=4;} }
    if(i==3){if(delateMessage(messageList[i],s))printf("MESSAGE NOT DELATED NETWORK PROBLEM TRY AGAIN\n");
	            else{printf("MESSAGE DELATED SUCESSFULLY\n");
		              i=4;//go back to inbox after delation}
	                }	      
		  

	      }
          

      
  } } 
    }while(i!=5 && i!=0);
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
       fclose(fp);
       fp=fopen("AdressBook","wb");
       fclose(fp);
       fp = fopen("AdressBook", "rb"); 
      perror("NoAdressBoook found i crate one");
    }
   j=0;

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
  //crate an array with all username 
  char  usernameListS[j][MAXLIMIT];
  trov=0;//set trov =1 when we find a matching 

  //read all username and print it 
  for(int k=0;k<j;j++){
    //read from socket and print for the user
    if (read(s,&usernameListS[k],sizeof(char[MAXLIMIT]))<0) {
	perror("write");
	return 1;}
    if(strcmp(usernameListS[k],toadd)==0)trov=1;
  }
  if (read(s,&type,sizeof(int))<0) {
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

//list all mesage sended to this user contact the MDS to get data, is an interactive function 
int listallmessage(int s,char username[MAXLIMIT]){
  int mdsRet;//return value from mds;
  int i=0;
  int type=8;//the type of call for get all messages destinated to a user;
  int inboxn=checkinbox(s);//recall now because inbox number could change in small time 
  //PackageData *c=malloc(1*sizeof(PackageData));
  PackageData messageList[inboxn];
    //wirte in the socket data for login or register
do{  if (write(s,&type,sizeof(type))<0) {
	perror("write");
	return 1;
     }
    printf("##############\n");
    printf("INBOX\n");
    printf("##############\n");
    printf("SElECT MESSAGE OR 0 TO GO BACK TO MAIN MENU\n");

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

    for(i=(inboxn-1);i>=0;i--){//print like that for have the last message frist
    printf("%d)Message from %s sended %s\n",(inboxn-i),messageList[i].from,messageList[i].timestamp);
  
      }

    scanf("%d",&i);
    
    if(i>=inboxn){printf("error message not found 404"); sleep(1); i=4;}//set a new value for i so you can back to all message;
    else if(i!=0){
      while(i<4){//if 3 the function reload all the message and you can list it again;
      //playback the message
      playback(messageList[(inboxn-i)].message);      
      printf("From %s \nTime:%s\n",messageList[(inboxn-i)].from,messageList[(inboxn-i)].timestamp);
      printf("1) for listen again \n");
      printf("2)to inoltrate the message\n");
      printf("3)to delete the message\n");
      printf("4)come back to inbox \n");
      printf("5)go back to main page\n");
      scanf("%d",&i);
     if(i==2){selectuserto(s,messageList[i].to); //inoltrate to a new user the message;
	      strcpy(messageList[i].from,username);
             if(send_PackageData(messageList[(inboxn-i)],s))printf("MESSAGE NOT SENT NETWORK PROBLEM TRY AGAIN \n");//sendMessage return 1 upon fail
	            else{printf("MESSAGE SENT SUCESSFULLY\n");
	            i=4;} }
    if(i==3){if(delateMessage(messageList[(inboxn-i)],s))printf("MESSAGE NOT DELATED NETWORK PROBLEM TRY AGAIN\n");
	            else{printf("MESSAGE DELATED SUCESSFULLY\n");
		          i=4;//go back to inbox after delation}
	            }	      
		  

	      }
          

      
  } } 
    }while(i!=5 && i!=0);
free(messageList);//good thing to do
return 0;//return 0 in case of success;
  
   

  


}
//send login data to socket return 1 for auth compleate 0 on failed
int login(userData u, int t,int  s ){
  int len;
  u.type=t; 
  //wirte in the socket data for login or register
  if (send_int(t,s)<0) {
	    perror("write");
	    exit(1);
      }
  //read the len of the username     
  len=strlen(u.username);
  //send  to the socket the len 
  if (send_int(len,s)<0) {
	    perror("write");
	    exit(1);
      }
  //send the username to the socket ad one for the endian 
  if (write(s,u.username,len+1)<0) {
	    perror("write");
	    exit(1);
      }
  //read the len of the password     
  len=strlen(u.password);
  //send  to the socket the len 
  if (send_int(len,s)<0) {
	    perror("write");
	    exit(1);
      }
  //send the password to the socket ad one for the endian 
  if (write(s,u.password,len+1)<0) {
	    perror("write");
	    exit(1);
      }

  // Read  from socket 1 for login or register done 0 for not 
  if (receive_int(&t,s)<0) {
	perror("read");
	exit(1); }

    return t;  

  }

  // send a new message to a user in an interactive way 
  int sendNew( char username[20], int s ){
        int i;
        PackageData tosend;
	      AudioDataf messagetosend;
        strcpy(tosend.from,username);
      	if(selectuserto(s,tosend.to)){//select the uset to send the data to
            perror("Problem with mds"); 
            return 1;}
      	time_t ltime; /* calendar time */
        ltime=time(NULL); /* get current cal time */
      	strcpy(tosend.timestamp,asctime( localtime(&ltime)));//Get a timestamp of the actual moment in a redable format;
        messagetosend = recordP();
        tosend.message=messagetosend;
        tosend.size=sizeof(tosend.message);
	      tosend.hash=hashCode(tosend);
        printf("%d",tosend.hash);
        tosend.type=6;
        do{ printf("1) send the message\n");
	          printf("2) listen the message before send it\n");
	          printf("3) to return to the main menu\n");
	          printf("4)change the destinatary user actualy is: %s \n",tosend.to);
            printf("5)go back to main menu \n");
	          scanf("%d",&i);


	          if(i==1){
	            printf("sending your message \n");
	            if(send_PackageData(tosend,s)){
                printf("MESSAGE NOT SENT NETWORK PROBLEM TRY AGAIN \n");//sendMessage return 1 upon fail
                return 1;}
	            else{printf("MESSAGE SENT SUCESSFULLY\n");
              printf("1)inoltrate this massage to another user\n");
		          printf("2)go back to the main menu\n");
		          scanf("%d",&i);
		             i=i+3;//smart move in this way if you have pressed 2 you go back and if you press 1 you inoltrate the message
                }
            }
	          if(i==2)playback(tosend.message);//playback the audio registered
	          if (i==4){selectuserto(s,tosend.to);
		         tosend.hash=hashCode(tosend);
	           } 
	           }while(i<5);

         return 0;

  }     


int main(int argc, char *argv[])
{
 if (argc >3) {
	      printf("\nUsage:\n\t%s <User> <Password>\nOr no argument and wait for prompt\n",argv[0]);
	      exit(1);
              }
//printf("\033[0;36m");   
printf("____________________________________________");
printf("Welocome to AudioMessage ");
printf("--------------------------------------------- \n");
//printf(“\033[0m”); 
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
    if ((hst=gethostbyname("localhost"))==NULL) {
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
             *afantastic.username=*argv[1];
             *afantastic.password=*argv[2];
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
  printf("%d inbox messages for you \n",checkinbox(s)); //removed for testing reason could couse problem
  printf("1) show the new messages \n");
  printf("2) send a new message \n");
  printf("3) add new contact in the address book \n");
  printf("4) search messages\n");
  printf("9) EXIT\n");
  scanf("%d",&i);
  switch (i) {
    case 1:{
	   listallmessage(s,afantastic.username);
     break;
	 }
    case 2 ://select a user and then send a message user can be select eihter by address book or by writeing is name ;
	 
      {if(sendNew(afantastic.username,s))perror("network problem with MDS");
       else printf("message sent");
       
      }//close case 2;
      break;
    case 3:{
	     if(addusertoadressbook(s)){perror("Error,Try Again");}}
       break;
	   
  
    case 4:
	     serchMessages(afantastic.username,s);
       break;
	   	   

     case 5:{
       printf("hidden option to test micrphone");
	     AudioDataf a;
       a= recordP();
       playback(a);}
       break;
	   
    	
    case 9:
          if(disconnect(s)){printf("error in disconection try again");i=5; }//disconnect from the mds return 1 in case of error
          break;
                
    default: printf("This option is not avaible \n"); 
        
    }
  }while(i!=9);
}
