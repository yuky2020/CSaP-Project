#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include "../lib/util.h"
//function to select a user form addressbook or from all user @param s:socket to MDS,@param *tmp string  to save the user to;
int selectuserto(int s, char *tmp)
{
  int len;          //lenght of evry char that is sended;
  int j, ru, i = 0; //ru is the nuber of user actualy in server
  printf("1)search from adressBook\n");
  printf("2)search from all user\n");
  scanf("%d", &i);
  printf("select wanted user by number\n");
  if (i == 1)
  {
    FILE *fp;                                      //file where to place adressBook;
    char usernameList[ADDRESSBOOKLIMIT][MAXLIMIT]; //list from adressBook

    if ((fp = fopen("AdressBook", "rb")) == NULL)
    {
      perror(" opening file");
      // Program exits if file pointer returns NULL.
      return 1;
    }
    j = 0;

    while (!feof(fp))
    { // while not end of file
      fread(usernameList[j], sizeof(char[MAXLIMIT]), 1, fp);
      printf("%d,%s\n", (j + 1), usernameList[j]);
      j++;
    }
    j--;        //becouse you encrement before re enter
    fclose(fp); //close the file for now
    int k;
    do
    {
      scanf("%d", &k);
      if (k > j)
        printf("this number is still not presnt");
    } while (k > j);
    strcpy(tmp, usernameList[(k - 1)]);
    //  free(usernameList);//fre the user list not useful anymore;
    return 0;
  }

  else if (i == 2)
  {               //retrive all user and selsct from the list
    int type = 9; //the type of call for this function
    if (send_int(type, s))
    {
      perror("write");
      return 1;
    }
    sleep(1);
    printf("Sto per ricevere dei dati \n");
    if (receive_int(&ru, s))
    {
      perror("write");
      return 1;
    }
    //at the lest there is one user
    printf("actualy registred users %d \n", ru);
    if (ru < 1)
    {
      perror("problem with MDS");
      return 1;
    }
    char usernamesist[ru][MAXLIMIT];
    sleep(1);
    //char usernameList[MAXLIMIT][MAXLIMIT]; here for testing pourpose
    //read all the username and print it
    for (int k = 0; k < ru; k++)
    {
      //read the lenght of the next string;
      if (read(s, &len, sizeof(int)) < 0)
      {
        perror("read");
        return 1;
      }
      //read from socket and print for the user
      if (read(s, usernamesist[k], len + 1) < 0)
      {
        perror("read");
        return 1;
      }
      usernamesist[k][len] = '\0'; /* Terminate the string! */
      printf("%d  %s\n", (k + 1), usernamesist[k]);
    }
    if (receive_int(&type, s) < 0)
    {
      perror("read");
      return 1;
    }
    //type is used to store the end of trasmission from MDS;
    if (type != 1)
    {
      perror("type");
      return 1;
    }
    type = 5; //end of trasmission for MDS
    if (send_int(type, s) < 0)
    {
      perror("write");
      return 1;
    }

    printf("select wanted user ");
    //actual_user enter the user wanted;
    do
    {
      scanf("%d", &i);
      i--;
      if (i > ru || i < 0)
        printf("This user is still not in list try again \n");
    } while (i > ru || i < 0);

    strcpy(tmp, usernamesist[i]);
    //free(usernameList);//fre the user list not useful anymore;
  }

  return 0;
}

//return number of massage stored actualy in Inbox
int checkinbox(int s)
{
  int ret, type = 7; //7 is the type for this call
  //write type in the socket
  if (send_int(type, s) < 0)
  {
    perror("write");
    exit(1);
  }
  //then the socket should return the number of massage
  if (receive_int(&ret, s) < 0)
  {
    perror("read");
    exit(1);
  }
  return ret;
}

//write in  the socket type=5,so Mds whill close child process;
int disconnect(int s)
{
  int type = 5;
  if (send_int(type, s) < 0)
  {
    perror("write");
    return 1;
  }
  return 0;
}
//Function to send a message to MDS @Param PackageData:Struct that contains the message,and other value see util.h @Param s socket conected  to MDS

//function to tell mds to delate a message
int delateMessage(PackageData todel, int s)
{
  int type = 10; // type for this call;
                 //wirte the type in the socket
  if (send_int(type, s) < 0)
  {
    perror("write");
    return 1;
  }

  if (send_PackageData(todel, s))
  {
    perror("send Message");
    return 1;
  }
  //check the return value from MDS;
  if (receive_int(&type, s) < 0)
  {
    perror("read");
    return 1;
  }
  if (type == 0)
    return 0;
  else
    return 1;
}

//add one user to the adressBook after check it it is or not enroled in the server
int addusertoadressbook(int s)
{
  FILE *fp; //file where to place adressBook;
  char usernameList[ADDRESSBOOKLIMIT][MAXLIMIT];
  char toadd[MAXLIMIT]; //username to add to the adressBook;
  int j, trov, i, ru;   //for save the number of user and for going trought it
  int len = 0;
  trov = 0;
  if ((fp = fopen("AdressBook", "rb")) == NULL)
  {
    fp = fopen("AdressBook", "wb");
    fclose(fp);
    fp = fopen("AdressBook", "rb");
    perror("NoAdressBoook found i create a new one");
  }
  j = 0;

  while (!feof(fp))
  { // while not end of file
    fread(usernameList[j], MAXLIMIT * sizeof(char), 1, fp);
    j++;
  }
  j--;        //becouse you encrement before re enter
  fclose(fp); //close the file for now

  printf("insert username to add\n");
  //the frist call is only for clean the buffer
  fgets(toadd, MAXLIMIT, stdin);
  fgets(toadd, MAXLIMIT, stdin);
  toadd[strlen(toadd)] = '\0';
  //check if the user is alredy in the adressBook;
  for (i = 0; i <= j; i++)
  {
    if (strcmp(usernameList[i], toadd) == 0)
    {
      printf("This user is alredy in the list");
      return 1;
    }
  }
  //check if the user exist() from server
  int type = 9; //the type of call for this function
  if (send_int(type, s))
  {
    perror("write");
    return 1;
  }
  //recive currently registred users
  if (receive_int(&ru, s))
  {
    perror("write");
    return 1;
  }
  if (ru <= 1)
  {
    perror("problem with MDS");
    return 1;
  }
  //allocate the space for the users list
  char usernamesist[ru][MAXLIMIT];

  //read all the username and add it to the list
  for (int k = 0; k < ru; k++)
  {
    //read the lenght of the next string;
    if (read(s, &len, sizeof(int)) < 0)
    {
      perror("read");
      return 1;
    }
    //read from socket and print for the user
    if (read(s, usernamesist[k], len + 1) < 0)
    {
      perror("read");
      return 1;
    }
    usernamesist[k][len] = '\0'; /* Terminate the string! */
    if (strcmp(toadd, usernamesist[k]) == 0)
      trov = 1;
  }
  if (receive_int(&type, s) < 0)
  {
    perror("read");
    return 1;
  }
  //type is used to store the end of trasmission from MDS;
  if (type != 1)
  {
    perror("type");
    return 1;
  }
  type = 5; //end of trasmission for MDS
  if (send_int(type, s) < 0)
  {
    perror("write");
    return 1;
  }
  //if i found it
  if (trov == 1)
  {

    fp = fopen("AdressBook", "ab");
    if (fwrite(toadd, MAXLIMIT * sizeof(char), 1, fp) < 0)
    {
      perror("error in adding the name ");
      return 1;
    }
    fclose(fp);
    printf("user added \n");
    return 0;
  }
  else
  {
    printf("no user with this name found try another one \n");
    return 0;
  }
}

//list all message sneded to the logged user return 1 on faiulure 0 on success
int getallmessage(int s, int inboxn, PackageData retMessageList[inboxn])
{
  int ctype, mdsRet; //Control type for sending;return value from mds;
  int i = 0;
  int type = 8; //the type of call for get all messages destinated to a user;
  //PackageData *c=malloc(1*sizeof(PackageData));
  //wirte in the socket data for login or register
  if (send_int(type, s) < 0)
  {
    perror("write");
    return 1;
  }
  for (int i = 0; i < inboxn; i++)
  {
    if (receive_int(&ctype, s) < 0)
    {
      perror("recive int ");
      return 1;
    }
    if (ctype != 6)
    {
      perror("sending is compromised");
      return 1;
    }
    if (recive_PackageData(&retMessageList[i], s))
    {
      perror("reciving hasent work");
      return 1;
    }
  }
  //after reding data read the return value
  if (receive_int(&mdsRet, s) < 0)
  {
    perror("read");
    return 1;
  }
  //check if is correct
  if (mdsRet != 5)
  {
    perror("Sending hasent work");
    //free(messageList);
    return 1;
  }
  //send doble check to Mds
  if (send_int(mdsRet, s) < 0)
  {
    perror("read");
    //free(messagelist);
    return 1;
  }

  return 0;
}
//show a message details and do some actions
int showMessagewithOptions(int s, char username[MAXLIMIT], PackageData todispaly)
{
  int i = 0;
  do
  {
    printf("From %s \nTime:%s\n", todispaly.from, todispaly.timestamp);
    printf("1) for listen the message \n");
    printf("2)to inoltrate the message\n");
    printf("3)to delete the message\n");
    printf("4)go back to main page\n");
    scanf("%d", &i);
    if (i == 1)
    {
      playback(todispaly.message);
    }
    if (i == 2)
    {
      selectuserto(s, todispaly.to); //inoltrate to a new user the message;
      strcpy(todispaly.from, username);
      todispaly.hash=hashCode(todispaly);
      if (send_PackageData(todispaly, s))
        printf("MESSAGE NOT SENT NETWORK PROBLEM TRY AGAIN \n"); //sendMessage return 1 upon fail
      else
      {
        printf("MESSAGE INOLTRATED SUCESSFULLY\n");
      }
    }
    if (i == 3)
    {
      if (delateMessage(todispaly, s))
        printf("MESSAGE NOT DELATED NETWORK PROBLEM TRY AGAIN\n");
      else
      {
        printf("MESSAGE DELATED SUCESSFULLY\n");
        i = 4; //go back to inbox after delation}
      }
    }

  } while (i < 4);
  return 0;
}
//list all mesage sended to this user contact the MDS to get data, is an interactive function
int listallmessage(int s, char username[MAXLIMIT])
{
  int j, i = 0;
  int inboxn = checkinbox(s); //recall now because inbox number could change in small time
  //PackageData *c=malloc(1*sizeof(PackageData));
  if (inboxn == 0)
  {
    printf("NO NEW MESSAGES FOUND, INBOX IS CLEAR \n");
  }
  else
  {
    PackageData messageList[inboxn];

    //start showing message

    printf("##############\n");
    printf("INBOX\n");
    printf("##############\n");
    printf("SElECT MESSAGE OR 0 TO GO BACK TO MAIN MENU\n");
    if (getallmessage(s, inboxn, messageList))
    {
      perror("problem with reading message");
      return 1;
    }
    for (j = (inboxn - 1); j >= 0; j--)
    { //print like that for have the last message frist
      printf("%d)Message from %s sended %s \n", (inboxn - j), messageList[j].from, messageList[j].timestamp);
    }
    //lets select a  message
    scanf("%d", &j);
    while (j > inboxn || j <= 0)
    {
      printf("error message not found try again \n");
      scanf("%d", &j);
    }
    j = (j - inboxn) * -1;
    if (showMessagewithOptions(s, username, messageList[j]))
      perror("problem in display the message");

  } //else close
  //not necessary since message are not allocated dinamicaly
  //free(messageList); //good thing to do                    //close the else of 0 or some message
  return 0; //return 0 in case of success;
}
//function to search for a message in the inbox ;//download the inbox is not the best way, but make possible to do search more time without recall MDS //same method used by some  email client
int serchMessages(char user[MAXLIMIT], int s)
{
  char toserchu[MAXLIMIT];      //the string to search for a user  ;
  char toserchd[2][TIMESTAMPS]; //the date between search
  int mdsRet;                   //return value from mds;
  int k, i, len, t = 0;         //for select among user or date serch and valid or not ;
  len = 0;
  int type = 8;               //the type of call for get all messages destinated to a user;
  int inboxn = checkinbox(s); //recall now because inbox number could change in small time
  int trov[inboxn];           //array to save the index of matching value
  //PackageData *c=malloc(1*sizeof(PackageData));
  PackageData messageList[inboxn];
  //wirte the type in the socket

  printf("##############\n");
  printf("Search for  messages\n");
  printf("##############\n");
  printf("1)search for a user messages\n");
  printf("2)search for messages in a date range\n");
  printf("3)serch for a user messages in a date range\n");
  scanf("%d", &k);
  if (k == 1 || k == 3)
    selectuserto(s, toserchu);
  if (k == 2 || k == 3)
  {
    printf("write the start date to search in format:es Sat Mar 25 06:10:10 1989  \n");
    scanf("%s", toserchd[0]);
    printf("write the final date to search \n");
    scanf("%s", toserchd[1]);
  }

  if (getallmessage(s, inboxn, messageList))
  {
    perror("problem reciving the message ");
    return 1;
  }
  for (i = (inboxn - 1); i >= 0; i--)
  { //print like that for have the last message frist
    t = 0;
    if (k == 1 || k == 3)
      if (strcmp(messageList[i].from, toserchu) == 0)
        t = 1; //check fot the user
    if (k == 2 || k == 3)
    {
      if (datecmp(messageList[i].timestamp, toserchd[0]) == 1 && datecmp(toserchd[1], messageList[i].timestamp))
        t = 1; //check for the date
      else
        t = 0;
    }
    if (t == 1)
    {
      trov[(len)] = i;
      len++;
      printf("%d)Message from %s sended %s\n", (len), messageList[i].from, messageList[i].timestamp);
    }
  }
  //se non ho trovato nulla esco
  if (len == 0)
  {
    printf("no message found for the search \n");
    return 0;
  }
  scanf("%d", &i);

  while (i > len || i <= 0)
  {
    printf("error message not found try again \n");
    scanf("%d", &i);
  }
  i = trov[i - 1];
  if (showMessagewithOptions(s, user, messageList[i]))
    perror("error in displaying the data ");

  return 0; //return 0 in case of success;
}
//send login data to socket return 1 for auth compleate 0 on failed
int login(userData u, int t, int s)
{
  int len;
  u.type = t;
  //wirte in the socket data for login or register
  if (send_int(t, s) < 0)
  {
    perror("write");
    exit(1);
  }
  //read the len of the username
  len = strlen(u.username);
  //send  to the socket the len
  if (send_int(len, s) < 0)
  {
    perror("write");
    exit(1);
  }
  //send the username to the socket ad one for the endian
  if (write(s, u.username, len + 1) < 0)
  {
    perror("write");
    exit(1);
  }
  //read the len of the password
  len = strlen(u.password);
  //send  to the socket the len
  if (send_int(len, s) < 0)
  {
    perror("write");
    exit(1);
  }
  //send the password to the socket ad one for the endian
  if (write(s, u.password, len + 1) < 0)
  {
    perror("write");
    exit(1);
  }

  // Read  from socket 1 for login or register done 0 for not
  if (receive_int(&t, s) < 0)
  {
    perror("read");
    exit(1);
  }

  return t;
}
//function to change password once logged in the system 0 on success 1 on error
int changepasswd(int s)
{
  int type = 11; //type for this call
  int len;
  int retValue = 1;
  char newPasswd[MAXLIMIT];
  char newPasswdConf[MAXLIMIT];
  do
  {
    printf("\n");
    printf("New Password: ");
    fgets(newPasswd, MAXLIMIT, stdin);
    printf("\n RE-type it again: ");
    fgets(newPasswdConf, MAXLIMIT, stdin);
    if (strcmp(newPasswd, newPasswdConf) != 0)
    {
      printf("\n try again password differs \n");
    }
    else
      printf("\n");

  } while (strcmp(newPasswd, newPasswdConf) != 0);
  len = strlen(newPasswd);
  //send  to the socket the type of the call
  if (send_int(type, s) < 0)
  {
    perror("write");
    return 1;
  }

  //send  to the socket the len
  if (send_int(len, s) < 0)
  {
    perror("write");
    return 1;
  }
  //send the password to the socket ad one for the endian
  if (write(s, newPasswd, len + 1) < 0)
  {
    perror("write");
    return 1;
  }

  // Read  from socket 1 for change passwd done or not 0 for not
  if (receive_int(&retValue, s) < 0)
  {
    perror("read");
    return 1;
  }
  return retValue;
}
// send a new message to a user in an interactive way
int sendNew(char username[20], int s)
{
  int i;
  PackageData tosend;
  AudioDataf messagetosend;
  strcpy(tosend.from, username);
  if (selectuserto(s, tosend.to))
  { //select the uset to send the data to
    perror("Problem with get a user");
    return 1;
  }
  time_t ltime;                                         /* calendar time */
  ltime = time(NULL);                                   /* get current cal time */
  strcpy(tosend.timestamp, asctime(localtime(&ltime))); //Get a timestamp of the actual moment in a redable format;
  messagetosend = recordP();
  tosend.message = messagetosend;
  tosend.size = sizeof(tosend.message);
  tosend.hash = hashCode(tosend);
  printf("%d", tosend.hash);
  tosend.type = 6;
  do
  {
    printf("1) send the message\n");
    printf("2) listen the message before send it\n");
    printf("3) to return to the main menu\n");
    printf("4)change the destinatary user actualy is: %s \n", tosend.to);
    printf("5)go back to main menu \n");
    scanf("%d", &i);

    if (i == 1)
    {
      printf("sending your message \n");
      if (send_PackageData(tosend, s))
      {
        printf("MESSAGE NOT SENT NETWORK PROBLEM TRY AGAIN \n"); //sendMessage return 1 upon fail
        return 1;
      }
      else
      {
        printf("MESSAGE SENT SUCESSFULLY\n");
        printf("1)inoltrate this massage to another user\n");
        printf("2)go back to the main menu\n");
        scanf("%d", &i);
        i = i + 3; //smart move in this way if you have pressed 2 you go back and if you press 1 you inoltrate the message
      }
    }
    if (i == 2)
      playback(tosend.message); //playback the audio registered
    if (i == 4)
    {
      selectuserto(s, tosend.to);
      tosend.hash = hashCode(tosend);
    }
  } while (i < 5);

  return 0;
}
//full removing message dstinated to this user
int fullwipemessage(int s)
{
  int retValue = 1;
  int type = 12; //type for this call;
  if (send_int(type, s) < 0)
  {
    perror("write");
    return 1;
  }
  // Read  from socket 1 for change passwd done or not 0 for not
  if (receive_int(&retValue, s) < 0)
  {
    perror("read");
    return 1;
  }
  return retValue;
}

int main(int argc, char *argv[])
{
  if (argc > 3)
  {
    printf("\nUsage:\n\t%s <User> <Password>\nOr no argument and wait for prompt\n", argv[0]);
    exit(1);
  }
  //printf("\033[0;36m");
  printf("____________________________________________\n");
  printf("Welocome to AudioMessage \n");
  printf("--------------------------------------------- \n");
  //printf(“\033[0m”);
  int s, t = 0;
  //inizialize userData
  userData afantastic;
  char hostname[MAXLIMIT];
  struct sockaddr_in saddr;
  struct hostent *hst;
  char port[6]; //used to read and store  the port
  FILE *config;
  //open the config file
  if ((config = fopen("config.txt", "r")) == NULL)
  {
    perror(" opening file");
    // Program exits if file pointer returns NULL.
    exit(1);
  }
  //skip the frist  string 
  fgets(hostname,20,config);
  // reads text until newline is encountered
  fgets(hostname, 20, config);
  //add terminaton
  hostname[strlen(hostname) - 2] = '\0';

  fgets(port, 6, config);
  //skip also port:

  //read port number

  fclose(config);

  // here for testing pourpose
  //printf("%s,%s", hostname, port);

  // Create the stream socket
  if ((s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
  {
    perror("socket");
    exit(1);
  }

  // Determine host address by its network name
  if ((hst = gethostbyname(hostname)) == NULL)
  {
    perror("gethostbyname");
    exit(1);
  }

  // Fill structure
  bcopy(hst->h_addr, &saddr.sin_addr, hst->h_length);
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(atoi(port));

  // Connect to other socket
  if (connect(s, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
  {
    perror("connect");
    exit(1);
  }
  puts("connect done");

  if (argc == 3)
  {
    t = 1;
    *afantastic.username = *argv[1];
    *afantastic.password = *argv[2];
  }

  do
  {
    if (t == 0)
    {
      printf("1)Login\n");
      printf("2)Register\n");
      scanf("%d", &t);
      getchar();
      printf("Please insert user:");
      fgets(afantastic.username, MAXLIMIT, stdin);
      printf("\nPlease insert password:");
      fgets(afantastic.password, MAXLIMIT, stdin);
    }

    if (!login(afantastic, t, s))
    {
      t = 0;
      printf("\nAuthFalied please try again\n");
    }
  } while (t == 0);

  printf("Welcome Back %s \n", afantastic.username);
  int i, ninbox;
  do
  {
    ninbox = checkinbox(s);
    printf("\n%d inbox messages for you \n", ninbox); //removed for testing reason could couse problem
    printf("1) show the new messages \n");
    printf("2) send a new message \n");
    printf("3) add new contact in the address book \n");
    printf("4) search messages\n");
    printf("5) settings\n");
    printf("9) EXIT\n");
    scanf("%d", &i);
    switch (i)
    {
    case 1:

      if (listallmessage(s, afantastic.username))
      {
        perror("error");
      }
      break;

    case 2: //select a user and then send a message user can be select eihter by address book or by writeing is name ;

    {
      if (sendNew(afantastic.username, s))
        perror("network problem with MDS");
      else
        printf("message sent");

    } //close case 2;
    break;
    case 3:
    {
      if (addusertoadressbook(s))
      {
        perror("Error,Try Again");
      }
    }
    break;

    case 4:
      serchMessages(afantastic.username, s);
      break;

    case 5:
    {
      int setOp = 0; //for selecting the option
      do
      {

        printf("\n SETTINGS \n");
        printf("1) test Microphone \n");
        printf("2) change password  \n");
        printf("3) full wipe of your inbox  \n");
        printf("4) go back to main menu\n");
        scanf("%d%*c", &setOp);

        switch (setOp)
        {
        case 1:
        {
          AudioDataf a;
          a = recordP();
          playback(a);
        }
        break;
        case 2:
          if (changepasswd(s))
          {
            perror("change passwd has not'work ");
          }
          else
            printf("password changed remeber it when relogin \n");
          break;
        case 3:
        {
          char fate;
          printf("THIS CANNNOT BE UNDONE ARE YOU SURE ? Y/N");
          scanf(" %c", &fate);
          if (fate == 'Y'){
            if (fullwipemessage(s))
            {
              perror("full wipe has not work");
            }
            else
            {
              printf("ALL MESSAGE GONE \n ");
            }}
        }
        break;
        case 4:
          break;

        default:
          printf("Error options not available ");
          break;
        }

      } while (setOp != 4);
    }

    break;

    case 9:
      if (disconnect(s))
      {
        printf("error in disconection try again");
        i = 5;
      } //disconnect from the mds return 1 in case of error
      break;

    default:
      printf("This option is not avaible \n");
    }
  } while (i != 9);
}
