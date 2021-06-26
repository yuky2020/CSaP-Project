#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include "../lib/util.h"
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#define SEM_NAME "/semaphorev"                            //is the prefix in the name of every semaphore used for VDR
#define SEM_NAMEUL "/semaphoreul"                         //is the name of the semaphore for the userlist
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) // "Permessi del semaforo"
#define INITIAL_VALUE 1                                   //intial value of the semaphore
#define VDRN 1                                            //Number of vdr used
//get the number of inbox message of a logged user;
int giveInbox(char usern[MAXLIMIT], int vdrIndex, int vdrs[VDRN])
{
   sem_t *sem;
   int vdrtype = 7;                         //7 is the type to write in the socket for this call
   int len = 0;                             //lenght of the string to send
   int inboxvdr = 0;                        //the buffer for the returning value
   char saddress[(MAXLIMIT + 10)] = {'\0'}; //the adress of the semphore for the i vdr
   sprintf(saddress, "%s%d", SEM_NAME, vdrIndex);
   sem = sem_open(saddress, O_RDWR); //open the semaphore
   if (sem == SEM_FAILED)
   {
      perror("sem_open failed");
      exit(EXIT_FAILURE);
   }
   sem_wait(sem); //lock the semaphore so you can write to the VDR socket without risk
   //write the type of request to the socket
   //write vdrtype to the socket
   if (send_int(vdrtype, vdrs[vdrIndex]) < 0)
   {
      perror("write");
      exit(1);
   }
   // send the len of the string
   len = strlen(usern);
   if (send_int(len, vdrs[vdrIndex]) < 0)
   {
      perror("write");
      exit(1);
   }
   //write username to the socket +1 for endian value
   if (write(vdrs[vdrIndex], usern, len + 1) < 0)
   {
      perror("write");
      exit(1);
   }
   //sleep(1);//give time to recive data and send the response from the i-th vdr process
   //read the result from actual vdr
   if (receive_int(&inboxvdr, vdrs[vdrIndex]) < 0)
   {
      perror("read");
      exit(1);
   }

   sem_post(sem); //unlock the semaphore "incresing the  counter"
   if (sem_close(sem) < 0)
   {
      perror("sem_close(3) failed");
      exit(0);
   }
   return inboxvdr;
}

//get the vdr for  a logged user
//naturaly you shuold always wait to check if other process is using the semaphore for vdr socket
int getvdrIndex(char username[MAXLIMIT], int vdrs[VDRN])
{
   int retvdr = 0;  //the return value from the vdr set to default to 0;
   int vdrtype = 9; //the type to send to the vdr in order to get back if the user is or not in it;
   int len = 0;     //for cech dimension of arrays of char (string)
   for (int i = 0; i < VDRN; i++)
   {
      sem_t *sem;
      char saddress[(MAXLIMIT + 10)] = {'\0'}; //the adress of the semphore for the i vdr
      sprintf(saddress, "%s%d", SEM_NAME, i);
      sem = sem_open(saddress, O_RDWR); //open the semaphore
      if (sem == SEM_FAILED)
      {
         perror("sem_open failed");
         exit(EXIT_FAILURE);
      }
      sem_wait(sem); //lock the semaphore so you can write to the VDR socket
      //write vdrtype to the socket
      if (send_int(vdrtype, vdrs[i]) < 0)
      {
         perror("write");
         exit(1);
      }
      //write username to the socket
      len = strlen(username);
      if (send_int(len, vdrs[i]) < 0)
      {
         perror("write");
         exit(1);
      }
      if (write(vdrs[i], username, len + 1) < 0)
      {
         perror("write");
         exit(1);
      }
      //sleep(1);//give time to recive data and send the response from the i-th vdr process,not useful removed
      //read the result from actual vdr
      if (receive_int(&retvdr, vdrs[i]) < 0)
      {
         perror("read");
         exit(1);
      }

      sem_post(sem); //unlock the semaphore "incresing counter"
      if (sem_close(sem) < 0)
      {
         perror("sem_close failed");
         exit(0);
      }
      //if retvdr==1 the user data is in that vdr so we can return here
      if (retvdr == 1)
         return i;
   }
   return 0; //if no vdr contains information return 0 you will use the frist;
}

//Delate a fiel from the vdr
int delatefromvdr(char username[MAXLIMIT], int vdrs[VDRN], int vdrIndex, int c)
{
   PackageData tosend; //the package to send to recive from client and send to the vdr for delation
   sem_t *sem;         //semaphore for lock the vdr while used
   int vdrToIndex;     //the index of the vdr of the to username;
   int vdrReturn;      //the final return value from vdr;
   //read the type from the sender
   if (read(c, &tosend.type, sizeof(tosend.type)) < 0)
   {

      perror("read");
      return 1;
   }
   if (tosend.type != 6)
   {
      perror("TYPE INCOMPATIBILITY");
      return 1;
   }

   //read the user from the data is sended
   if (read(c, &tosend.from, sizeof(tosend.from)) < 0)
   {

      perror("read");
      return 1;
   }
   //read the user to the message is sended
   if (read(c, &tosend.to, sizeof(tosend.to)) < 0)
   {
      perror("read");
      return 1;
   }
   //read the size of audio data
   if (read(c, &tosend.size, sizeof(tosend.size)) < 0)
   {
      perror("read");
      return 1;
   }
   //read the audioData
   if (read(c, &tosend.message, tosend.size) < 0)
   {
      perror("read");
      return 1;
   }

   //read the hash
   if (read(c, &tosend.hash, sizeof(tosend.hash)) < 0)
   {
      perror("read");
      return 1;
   }
   //read timestamp
   if (read(c, &tosend.timestamp, sizeof(tosend.timestamp)) < 0)
   {
      perror("read");
      return 1;
   }
   //check if the hash is still valid
   if (tosend.hash != hashCode(tosend))
   {
      perror("hash is different");
      return 1;
   }
   //ceck if the username is =to in this case we are sure is a user try to remove is messages ;
   if (strcmp(username, tosend.to) != 0)
   {
      perror("sombody is try to remove a message as anoter user");
      return 1;
   }
   //check which is the vdr for the to user ;
   vdrToIndex = getvdrIndex(tosend.to, vdrs);
   //now open and lock the semaphore
   char saddress[(MAXLIMIT + 10)] = {'\0'}; //the adress of the semphore for the i vdr
   sprintf(saddress, "%s%d", SEM_NAME, vdrToIndex);
   sem = sem_open(saddress, O_RDWR); //open the semaphore
   if (sem == SEM_FAILED)
   {
      perror("sem_open failed");
      exit(EXIT_FAILURE);
   }
   sem_wait(sem);    //lock the semaphore so you can write to the VDR socket without risk
   tosend.type = 10; //because is a delate operation

   //now send the data trought socket to the vdr and serialize it
   //when we send the type we  trigger the vdr to delate the data;
   if (write(vdrs[vdrToIndex], &tosend.type, sizeof(tosend.type)) < 0)
   {
      perror("write");
      return 1;
   }

   if (write(vdrs[vdrToIndex], &tosend.from, sizeof(tosend.from)) < 0)
   {
      perror("write");
      return 1;
   }

   if (write(vdrs[vdrToIndex], &tosend.to, sizeof(tosend.to)) < 0)
   {
      perror("write");
      return 1;
   }

   if (write(vdrs[vdrToIndex], &tosend.size, sizeof(tosend.size)) < 0)
   {
      perror("write");
      return 1;
   }

   if (write(vdrs[vdrToIndex], &tosend.message, tosend.size) < 0)
   {
      perror("write");
      return 1;
   }

   if (write(vdrs[vdrToIndex], &tosend.hash, sizeof(tosend.hash)) < 0)
   {
      perror("write");
      return 1;
   }

   if (write(vdrs[vdrToIndex], &tosend.timestamp, sizeof(tosend.timestamp)) < 0)
   {
      perror("write");
      return 1;
   }

   sem_post(sem); //unlock the semaphore "incresing the  counter"
   if (sem_close(sem) < 0)
   {
      perror("sem_close failed");
      exit(0);
   }

   sleep(1); // wait for get a response from vdr ;
   //read response from vdr
   if (read(vdrs[vdrToIndex], &vdrReturn, sizeof(vdrReturn)) < 0)
   {
      perror("read");
      return 1;
   }
   //then return 0 if everything goes fine
   if (vdrReturn == 1)
      return 0;
   return 1;
}
int putalluser(int c)
{
   int len = 0; //len for  string
   FILE *fp;
   int n = 0;                          //to save the number of actual user registered
   sem_t *sem;                         //semaphore for lock the userlist file  while used
   sem = sem_open(SEM_NAMEUL, O_RDWR); //open the semaphore
   if (sem == SEM_FAILED)
   {
      perror("sem_open failed");
      exit(EXIT_FAILURE);
   }
   //lock the semaphore
   //this part need a semaphore because this operation is plausible done by more client concurently
   sem_wait(sem);
   //read the number of user actualy registered
   if ((fp = fopen("usersListN", "rb")))
   {
      fread(&n, sizeof(int), 1, fp);
      fclose(fp);
   }
   else
   {
      fclose(fp);
      return 0;
   }
   //open all user list
   if (!(fp = fopen("usersList", "rb")))
   {
      fclose(fp);
      return 1;
   }
   char tmp[n][MAXLIMIT]; //to store a  username files;
   int i = 0;
   while (!feof(fp) && i < n)
   {
      //write the new user in the user list
      fread(tmp[i], sizeof(char[MAXLIMIT]), 1, fp);
      i++;
   }
   i--;           //becouse we increment before check;
   fclose(fp);    //close the file
   sem_post(sem); //increment the counter
   if (sem_close(sem) < 0)
   { //close the semaphore
      perror("sem_close failed");
      exit(0);
   }
   //write the number of username you are about to send ;
   if (send_int(n, c))
   {
      perror("write");
      return 1;
   }

   for (int j = 0; j <= i; j++)
   {
      len = strlen(tmp[j]);
      //write the len as described in
      if (write(c, &len, sizeof(int)) < 0)
      {
         perror("write");
         return 1;
      }

      if (write(c, tmp[j], strlen(tmp[j]) + 1) < 0)
      {
         perror("write");
         return 1;
      }
   }
   i = 1; //send 1 to let know the client that you have done
   if (send_int(i, c) < 0)
   {
      perror("write");
      return 1;
   }
   //wait aknowleg from client
   if (receive_int(&i, c) < 0)
   {
      perror("read");
      return 1;
   }
   //check return from client value anyway dont do nothing
   if (i != 5)
      perror("client dont respond");
   // free(tmp);
   return 0;
}

int storeMessage(PackageData tosend, int vdrs[VDRN])
{
   sem_t *sem;        //semaphore for lock the vdr while used
   int vdrToIndex;    //the index of the vdr of the to username;
   int vdrReturn = 0; //the final return value from vdr;
   //check which is the vdr for the to user ;
   vdrToIndex = getvdrIndex(tosend.to, vdrs);
   //now open and lock the semaphore
   char saddress[(MAXLIMIT + 10)] = {'\0'}; //the adress of the semphore for the i vdr
   sprintf(saddress, "%s%d", SEM_NAME, vdrToIndex);
   sem = sem_open(saddress, O_RDWR); //open the semaphore
   if (sem == SEM_FAILED)
   {
      perror("sem_open failed");
      exit(EXIT_FAILURE);
   }
   sem_wait(sem); //lock the semaphore so you can write to the VDR socket without risk
   //now send the data trought socket to the vdr and serialize it
   //when we send the type we also trigger the vdr to save the data;
   if (send_PackageData(tosend, vdrs[vdrToIndex]))
   {
      perror("error sending data");
      goto fine;
   }

   if (receive_int(&vdrReturn, vdrs[vdrToIndex]) < 0)
   {
      perror("read");
      vdrReturn = 0;
   }

fine:
   sem_post(sem); //unlock the semaphore "incresing the  counter"
   if (sem_close(sem) < 0)
   {
      perror("sem_close failed");
      exit(0);
   }
   //then return 0 if everything goes fine
   if (vdrReturn == 1)
      return 0;
   return 1;
}

//reciv a message from the client and ask vdr to store it @param username is the username that is sending @param vdrs[VDRN] is the list of vdr socket, c is the socket to comunicate with client
int ReciveMessage(char username[MAXLIMIT], int vdrs[VDRN], int c)
{
   PackageData tosend; //the package  to recive from client and send to the vdr
   if (recive_PackageData(&tosend, c))
   {
      perror("problem while sending to MDS");
      return 1;
   }
   printf("Message recived now i store it in the delegated  vdr");
   if (storeMessage(tosend, vdrs))
      return 1;
   else
      return 0;
}
//send a message to the vdr for store it

//function to send all message destinated to a user; to that user;
int getClientMessages(char username[MAXLIMIT], int vdrIndex, int vdrs[VDRN], int c)
{
   int vdrToType = 8;                                //vdr type for this operation
   int vdrRet, type, clientRet;                      //return value from vdr;and client
   int inboxN = giveInbox(username, vdrIndex, vdrs); //number of message to read;
   PackageData tmp;                                  //create the structure to conserve the data, an array is plausible but not need since we resend all to client
   sem_t *sem;                                       //poisix semaphore for comunicate with vdr without problem ;
   char saddress[(MAXLIMIT + 10)] = {'\0'};          //the adress of the semphore for the i vdr
   sprintf(saddress, "%s%d", SEM_NAME, vdrIndex);
   sem = sem_open(saddress, O_RDWR); //open the semaphore
   if (sem == SEM_FAILED)
   {
      perror("sem_open failed");
      exit(EXIT_FAILURE);
   }
   sem_wait(sem); //lock the semaphore so you can write to the VDR socket without risk
   //send the type to the vdr
   if (send_int(vdrToType, vdrs[vdrIndex]) < 0)
   {
      perror("write");
      return 1;
   }
   // send the username to the vdr ;
   if (send_int(strlen(username), vdrs[vdrIndex]) < 0)
   {
      perror("write");
      return 1;
   }
   if (write(vdrs[vdrIndex], username, strlen(username) + 1) < 0)
   {
      perror("write");
      return 1;
   }
   for (int i = 0; i < inboxN; i++)
   {
      //read the data for every message package
      //i will recive type 6 before for indicate that a package data is sended
      if (receive_int(&type, vdrs[vdrIndex]) < 0)
      {
         perror("recive int ");
         return 1;
      }
      if (type != 6)
      {
         perror("sending is compromised");
         return 1;
      }
      if (recive_PackageData(&tmp, vdrs[vdrIndex]))
      {
         perror("problem reciving the message from the vdr");
         return 1;
      }

      //now sending the data to the client
      if (send_PackageData(tmp, c))
      {
         perror("problem in sending a message to the client");
         return 1;
      }
   }

   if (receive_int(&vdrRet, vdrs[vdrIndex]) < 0)
   {
      perror("Last read operation failed");
      return 1;
   }

   if (vdrRet != 5)
   {
      perror("Value error on last read problem with vdr");
      return 1;
   }
   sem_post(sem); //unlock the vdr semaphore "incresing the  counter"
   //close the semaphore good pratice
   if (sem_close(sem) < 0)
   {
      perror("sem_close failed");
      exit(0);
   }
   //need to come back at this point
   //at this point we can send the return value also to the client for let know that sending is over
   if (send_int(vdrRet, c) < 0)
   {
      perror("write");
      return 1;
   }
   //and wait for a double check from client
   if (receive_int(&clientRet, c) < 0)
   {
      perror("read");
      return 1;
   }
   if (clientRet != 5)
   {
      perror("clientRet value not valid");
      return 1;
   }

   return 0;
}

//registration of a user
int registeru(userData afantasticuser)
{
   FILE *fp;
   int n = 0;                          //number of user in the user list
   sem_t *sem;                         //semaphore for lock the userlist file  while used
   sem = sem_open(SEM_NAMEUL, O_RDWR); //open the semaphore
   if (sem == SEM_FAILED)
   {
      perror("sem_open failed");
      exit(EXIT_FAILURE);
   }

   // save the address of the file
   char address[(MAXLIMIT + 20)] = {'\0'};
   sprintf(address, "users/%s.user", afantasticuser.username);
   //check if the file alredy exist and if it is the case return 0
   if ((fp = fopen(address, "rb")))
   {
      fclose(fp);
      return 0;
   }
   //now open and  write on the file
   fp = fopen(address, "wb");
   if (fp == NULL)
   {
      perror("file open");
      return 0;
   }

   fwrite(afantasticuser.password, sizeof(char[MAXLIMIT]), 1, fp);
   fclose(fp);
   //lock the semaphore
   //this part need a semaphore because this operation is plausible done by more client concurently
   sem_wait(sem);
   //open all user list number and read the nuber of user registrated if there isnt go over
   if ((fp = fopen("usersListN", "rb")))
   {
      fread(&n, sizeof(int), 1, fp);
      fclose(fp);
   }
   //open now in write mode
   if (!(fp = fopen("usersListN", "wb")))
   {
      fclose(fp);
      return 0;
   }
   n = n + 1;
   fwrite(&n, sizeof(int), 1, fp);
   fclose(fp);

   //open all user list
   if (!(fp = fopen("usersList", "ab")))
   {
      fclose(fp);
      return 0;
   }
   //write the new user in the user list
   fwrite(afantasticuser.username, sizeof(char[MAXLIMIT]), 1, fp);
   fclose(fp);    //close the file
   sem_post(sem); //decrement the counter
   if (sem_close(sem) < 0)
   { //close the semaphore
      perror("sem_close failed");
      exit(0);
   }

   return 1;
}
//autenticate a user
int autenticate(userData afantasticuser)
{

   FILE *fp;

   // save the address of the file
   char address[(MAXLIMIT + 20)] = {'\0'};
   sprintf(address, "users/%s.user", afantasticuser.username);
   //check if the file alredy exist and if it is the case return 0
   fp = fopen(address, "rb");
   if (fp == NULL)
   {
      perror("no such file");
      return 0;
   }
   char tmp[MAXLIMIT];
   fread(tmp, sizeof(tmp), 1, fp);
   if (!strcmp(afantasticuser.password, tmp))
      return 1;
   else
      return 0;
   fclose(fp);
}

//we do all the work with a single client in this function,is used by a child process
//parameter are the int c that identify  the scoket for the client and vdrs[VDRN] that are a list of socket vdr
void dowork(int c, int vdrs[VDRN])
{
   //@var User to login or register
   userData afantasticuser;
   int type, result, tentatives = 0;
   int vdrIndex; //the index of vdr to use
   int len;      //used to take care of the lens of the string
   //client can try to acess five times then for security reason connection is abortefor security reason connection is abortedd
   //read from Stream Socket
   do
   {
      //read the type of the request
      if (receive_int(&afantasticuser.type, c) < 0)
      {
         perror("read");
         exit(1);
      }
      //get the len of the username
      if (receive_int(&len, c) < 0)
      {
         perror("read");
         exit(1);
      }
      //get the username
      if (read(c, &afantasticuser.username, len + 1) < 0)
      {
         perror("read");
         exit(1);
      }
      afantasticuser.username[len] = '\0'; //add termination;
                                           //get the len of password
      if (receive_int(&len, c) < 0)
      {
         perror("read");
         exit(1);
      }
      //get the password the +1 is for  the endian value
      if (read(c, &afantasticuser.password, len + 1) < 0)
      {
         perror("read");
         exit(1);
      }
      afantasticuser.password[len] = '\0'; //add termination;

      if (afantasticuser.type == 1)
         result = autenticate(afantasticuser);
      else
      {
         if (afantasticuser.type == 2)
            result = registeru(afantasticuser);
         else
         {
            perror("Malformetted data from client(security allert: attack with a row socket is plausible )");
            exit(1);
         }
      }

      //write result to the stream
      if (send_int(result, c) < 0)
      {
         perror("write");
         exit(1);
      }
      tentatives++;
      if (tentatives >= 5)
      {
         //close stream if client cant log in in five times kill also the child process
         shutdown(c, 2);
         close(c);
         exit(1);
      }
   } while (result != 1);

   //from now we are logged so the server can start recive varius type of packages
   //lets check if we have alredy a vdr for this client and if not select one randomicaly;
   vdrIndex = getvdrIndex(afantasticuser.username, vdrs);

   do
   {
      type = 0;
      if (receive_int(&type, c) < 0)
      {
         perror("read type");
         exit(1);
      }
      else
      {

         if (type == 7) //if the type is 7 client want the number of inbox message  ;
         {
            int inbox = 0;
            inbox = giveInbox(afantasticuser.username, vdrIndex, vdrs); //this function return the number of inbox Audio message of a user;
            if (send_int(inbox, c) < 0)
            {
               perror("write");
               exit(1);
            }
         }

         if (type == 6) // if the type is 6 client want to send a message to another user;
         {
            if (ReciveMessage(afantasticuser.username, vdrs, c))
            {
               perror("problem while reciving the message");
            }
            type = 6;
         }

         if (type == 8) //if the type of the call is 8 the client is asking for is messages;
         {
            int isDone = 0;
            //function return 1 if there is a problem
            if (getClientMessages(afantasticuser.username, vdrIndex, vdrs, c))
            {
               perror("client recive wrong message");
               isDone = 1;
            }
         }

         if (type == 9) //if the type of call is 9 you are asking for all user;
         {
            if (putalluser(c))
            {
               perror("cant send users to client ");
            }
         }

         if (type == 10) //if the type of call is 10 you are asking for a delate from vdr;
         {
            int isDel = 0;
            if (delatefromvdr(afantasticuser.username, vdrs, vdrIndex, c))
            {
               perror("cant send users to client ");
               isDel = 1;
            }
            //write the result to the socket 0 for success 1 for error
            if (write(c, &isDel, sizeof(int)) < 0)
            {
               perror("write");
               exit(1);
            }
         }

         if (type == 5)
            printf("CHILD-CLOSE USER-LOGOUT  REQUESTED \n");

         if (type != 0 && (type < 5 || type > 10))
         {
            perror("Malicius client is plausible now i kill this child");
            printf("%d", type);
            type = 5;
         }
      }

   } while (type != 5); //type 5 close the connection with the client  //can be used only when the client want do disconect

   printf("Connection is getting closed");
   shutdown(c, 2);
   close(c);
}

int main()
{

   sem_t *semvdr[VDRN];            //semphore to prevent two client from write to the same vdr using poisix semphore because more easy to menage
   sem_t *semusl;                  //semaphore to prevent two client from write to userlist and userlistn
   int s, c, len, vdr, vdrs[VDRN]; //vdr is for the master socket of VDRS while on aceptance you have filled the array vdrs[VDRN],s is for the master socket for client while on aceptance you have the socket c
   struct sockaddr_in saddr;
   int ops[3];
   int addr;
   int running;    //number of running client
   int maxspawn;   //max number of  running client as far as Mds process is started;
   int runningvdr; //number of  vdr actualy connected
   //create semaphores for vdr
   for (int j = 0; j < VDRN; j++)
   {
      char saddress[(MAXLIMIT + 10)] = {'\0'}; //the adress of the semphore for the j vdr
      sprintf(saddress, "%s%d", SEM_NAME, j);

      // create, initialize semaphore
      semvdr[j] = sem_open(saddress, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);
      if (semvdr[j] == SEM_FAILED)
      {
         perror("sem_open error");
         exit(EXIT_FAILURE);
      }
   }
   semusl = sem_open(SEM_NAMEUL, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);
   if (semusl == SEM_FAILED)
   {
      perror("sem_open error");
      exit(EXIT_FAILURE);
   }

   //create the stream socket for the VDRs
   if ((vdr = socket(AF_INET, SOCK_STREAM, 0)) < 0)
   {
      perror("socket");
      exit(1);
   }
   if (setsockopt(vdr, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
   {
      perror("setsockopt(SO_REUSEADDR) failed");
      exit(1);
   }

   puts("client socket done waiting for all vdr connection and echo each one ");

   saddr.sin_family = AF_INET;
   saddr.sin_port = htons(16000);
   saddr.sin_addr.s_addr = INADDR_ANY;

   if (bind(vdr, (struct sockaddr *)&saddr, sizeof(saddr)))
   {
      perror("bind");
      exit(1);
   }
   puts("bind done");

   if (listen(vdr, 5))
   {
      perror("listen");
      exit(1);
   }
   puts("listen done waiting connection");

   while (runningvdr < VDRN)
   {
      len = sizeof(saddr);
      if ((vdrs[runningvdr] = accept(vdr, (struct sockaddr *)&saddr, &len)) < 0)
      {
         perror("accept");
         exit(1);
      }
      addr = *(int *)&saddr.sin_addr;
      printf("accept - connection from vdr %d.%d.%d.%d\n",
             (addr & 0xFF), (addr & 0xFF00) >> 8, (addr & 0xFF0000) >> 16,
             (addr & 0xFF000000) >> 24);
      //type for the echo call to a vdr process
      int echo = 1;
      //write the echo in the socket
      if (send_int(echo, vdrs[runningvdr]) < 0)
      {
         perror("write");
         exit(1);
      }
      sleep(1); //time to get the result write in the socket by vdr
      //read from the socket the value
      if (receive_int(&echo, vdrs[runningvdr]) < 0)
      {
         perror("write");
         exit(1);
      }

      //ceck if the response is valid and if its not close the connection
      if (echo != 1)
      {
         close(vdrs[runningvdr]);
      }
      else
         runningvdr++;
   }

   //create the stream socket for the client
   if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
   {
      perror("socket");
      exit(1);
   }
   if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
   {
      perror("setsockopt(SO_REUSEADDR) failed");
      exit(1);
   }
   puts("client socket done");

   saddr.sin_family = AF_INET;
   saddr.sin_port = htons(20000);
   saddr.sin_addr.s_addr = INADDR_ANY;

   if (bind(s, (struct sockaddr *)&saddr, sizeof(saddr)))
   {
      perror("bind");
      exit(1);
   }
   puts("bind done");

   if (listen(s, 5))
   {
      perror("listen");
      exit(1);
   }
   puts("listen done");

   running = 0;
   maxspawn = 0;

   while (1)
   {
      len = sizeof(saddr);
      if ((c = accept(s, (struct sockaddr *)&saddr, &len)) < 0)
      {
         perror("accept");
         exit(1);
      }
      addr = *(int *)&saddr.sin_addr;
      printf("accept - connection from  %d.%d.%d.%d\n",
             (addr & 0xFF), (addr & 0xFF00) >> 8, (addr & 0xFF0000) >> 16,
             (addr & 0xFF000000) >> 24);
      switch (fork())
      {
      case 0:
         close(s);
         dowork(c, vdrs);
         exit(0);
      case -1:
         perror("fork");
         break;
      default:
         close(c);
         running += 1;
         if (running > maxspawn)
         {
            maxspawn = running;
            printf("== Max:%d\n", maxspawn);
         }
         while (waitpid(-1, NULL, WNOHANG) > 0)
            running -= 1;
         break;
      }
   }
   // close  client "Master socket"
   close(s);
   //close vdr "Master socket"
   close(vdr);
   //unlink and destroy the usersList semaphore
   if (sem_unlink(SEM_NAMEUL) < 0) //unlink the semaphore
      perror("sem_unlink failed");
   sem_destroy(semusl);
   //close each vdr socket
   for (int i = 0; i <= VDRN; i++)
   {
      close(vdrs[i]);
   }
   //unlink and destroy vdr named semaphore
   for (int j = 0; j < VDRN; j++)
   {
      char saddress[(MAXLIMIT + 10)] = {'\0'}; //the adress of the semphore for the j vdr
      sprintf(saddress, "%s%d", SEM_NAME, j);

      if (sem_unlink(saddress) < 0) //unlink the semaphore
         perror("sem_unlink failed");
      sem_destroy(semvdr[j]); //destroy the semaphore after usage good pratice
   }
}
