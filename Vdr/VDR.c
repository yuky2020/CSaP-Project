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
int checkuser(char user[MAXLIMIT])
{
    FILE *fp;
    char address[(MAXLIMIT + 11)] = {'\0'};

    sprintf(address, "users/%sInbox", user);
    //check if the file  exist and if it is return 1 else return 0;
    if ((fp = fopen(address, "rb")))
        return 1;
    return 0;
}
//get number of message in inbox for a user ;
int getinbox(char user[MAXLIMIT])
{
    int ninbox = 0;
    FILE *fp;
    // save the address of the file the name is the usename of the reciver
    char address[(MAXLIMIT + 11)] = {'\0'};
    sprintf(address, "users/%sInbox", user);
    //check if the file  exist and if it is read the number of inbox then close the file
    if ((fp = fopen(address, "rb")))
    {
        if (fread(&ninbox, sizeof(int), 1, fp) < 0)
        {
            perror("read");
            return 0;
        }
        printf("%d", ninbox);
        fclose(fp);
    }
    else
    {
        printf("user not found %s", user);
    }

    return ninbox;
}
//function to return the message stored in a file by is username

int readandsendMessages(char user[MAXLIMIT], int s)
{
    int closetype = 5;
    int i = getinbox(user);
    FILE *fp;
    PackageData tmp; //package data where to store data while reading and before check
    // save the address of the file the name is the usename of the reciver
    char address[(MAXLIMIT + 11)] = {'\0'};
    sprintf(address, "users/%sData", user);
    //open in read mode
    fp = fopen(address, "rb");
    if (fp == NULL)
    {
        perror("error open");
        return 1;
    }
    //Start to reading and sending all PackageData destinated to  user ones to ones;also deserialize it
    for (; i >= 1; i--)
    {
        if (read_PackageData(&tmp, fp))
        {
            perror("read PackageData");
            return 1;
        }
        if (send_PackageData(tmp, s))
        {
            perror("send Pakage data");
            return 1;
        }
    }
    //close the file because we have done with it
    fclose(fp);
    //write the closetype inide the socket so Mds know that message sending is over   //write the closetype inide the socket so Mds now that message sending is over   //write the closetype inide the socket so Mds now that message sending is over   //write the closetype inide the socket so Mds now that message sending is over   //write the closetype inide the socket so Mds now that message sending is over   //write the closetype inide the socket so Mds now that message sending is over   //write the closetype inide the socket so Mds now that message sending is over
    if (send_int(closetype, s) < 0)
    {
        perror("write the closetype");
        return 1;
    }
    //if evrything goes fine return 1
    return 0;
}

//function to store a message to a file and add 1 to ninbox
int storetofile(PackageData tostore)
{
    int ninbox = 0;
    FILE *fp;
    // save the address of the file the name is the usename of the reciver
    char address[(MAXLIMIT + 11)] = {'\0'};

    sprintf(address, "users/%sData", tostore.to);
    //open in append mode
    fp = fopen(address, "ab");
    if (fp == NULL)
    {
        perror("error open");
        return 1;
    }
    printf("wirte a messagge on the disk");
    //store the package data
    if (store_PackageData(tostore, fp))
    {
        perror("store file problem ");
        return 1;
    }
    fclose(fp);
    printf("ready to write on inbox file ");
    sprintf(address, "users/%sInbox", tostore.to);
    //check if the file alredy exist and if it is read the number of inbox then close the file
    printf(" a new message inserted ");
    if ((fp = fopen(address, "rb")))
    {
        if (fread(&ninbox, sizeof(int), 1, fp) < 0)
        {
            perror("read");
            return 1;
        }
        fclose(fp);
    }
    //increment the number of inbox
    ninbox++;
    //now open and  write on the file the nof inbox
    fp = fopen(address, "wb");
    if (fp == NULL)
    {
        perror("error opening");
        return 1;
    }

    if (fwrite(&ninbox, sizeof(int), 1, fp) < 0)
    {
        perror("write to file");
        return 1;
    }
    fclose(fp);
    //finaly return 0 if evrything goes fine
    return 0;
}

//function to store a Message from MDS
int storeMessage(int s)
{
    printf("Reciving a message ");
    PackageData tostore;
    if (recive_PackageData(&tostore, s))
    {
        perror("Error in reciving data");
        return 1;
    }

    //finaly store the data
    if (storetofile(tostore))
    {
        perror("store");
        return 1;
    }
    //then return 0 if everything goes fine
    return 0;
}
int delateallMessages(char username[MAXLIMIT])
{
    FILE *fp;
    //save the address of the data
    char address[(MAXLIMIT + 11)] = {'\0'};
    sprintf(address, "users/%sData", username);
    //and the one of the inbox file
    char addressInbox[(MAXLIMIT + 11)] = {'\0'};
    sprintf(addressInbox, "users/%sInbox", username);
    fp = fopen(address, "rb");
    if (fp == NULL)
    {
        perror("error open");
        return 1;
    }
    else
        fclose(fp);

    //destroy the files
    fclose(fopen(address, "wb"));
    fclose(fopen(addressInbox, "wb"));
    return 0;
}

//delate a message alredy store for a user
int delateMessage(PackageData todel)
{
    //inizializethe read of all the package for this user;
    //read number of message for this user
    int j = getinbox(todel.to);
    int i = 0;
    int isdel = 0; //if i have found the file to delate or not
    FILE *fp;
    PackageData tmp[j]; //package data where to store data while reading and before check
    // save the address of the datafile the name is the usename of the reciver
    char address[(MAXLIMIT + 11)] = {'\0'};
    sprintf(address, "users/%sData", todel.to);
    //save the addtess of the inbox file of the reciver
    char addressInbox[(MAXLIMIT + 11)] = {'\0'};
    sprintf(addressInbox, "users/%sInbox", todel.to);
    //open in read mode
    fp = fopen(address, "rb");
    if (fp == NULL)
    {
        perror("error open");
        return 1;
    }
    //Start to reading and sending all PackageData destinated to  user ones to ones;also deserialize it
    for (; i < j; i++)
    {
        //read the package data from the file one by one
        read_PackageData(&tmp[i], fp);
    }
    //now i have to all the data stored and can delate the file_data
    fclose(fp);
    //destroy the files
    fclose(fopen(address, "wb"));
    fclose(fopen(addressInbox, "wb"));
    for (i = 0; i < j; i++)
    {
        if (strcmp(todel.timestamp, tmp[i].timestamp) == 0 && todel.hash == tmp[i].hash)
            isdel = 1;
        else
        { //store the data to the file
            if (storetofile(tmp[i]))
            {
                perror("store");
                return 1;
            }
        }
    }
    //at this point we have recreated the file without the todel PackageData
    //check if it was in the old data if is not return one else 0
    if (isdel == 0)
        return 1;
    return 0;
}
int main(int argc, char *argv[])

{

    int s, socret;       //s is for the socket ,socret is for returning some information to the socket
    int isConnected = 0; //if the socket is connected
    struct sockaddr_in saddr;
    struct hostent *hst;
    char hostname[MAXLIMIT];
    char port[6];
    int type; // indicates the type of request from MDS 1 is a echo request 2 is for a audio message
    FILE *config;

    //open config file
    if ((config = fopen("config.txt", "r")) == NULL)
    {
        perror(" opening file");
        // Program exits if file pointer returns NULL.
        exit(1);
    }
    //skip the string hostname:
    fseek(config, 10 * sizeof(char), SEEK_CUR);
    // reads text until newline is encountered
    fscanf(config, "%[^\n]", hostname);
    //skip also port:
    fseek(config, 7 * sizeof(char), SEEK_CUR);
    //read port number
    fscanf(config, "%[^\n]", port);
    fclose(config);
restart://this label is used when mds crash to restart the connection with the new mds
    if (isConnected == 1)
    {
        close(s);
        isConnected = 0;
    }

    // Create the stream socket
    if ((s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        perror("socket");
        exit(1);
    }
    puts("socket done");

    // Determine host address by its network name
    if ((hst = gethostbyname(hostname)) == NULL)
    {
        perror("gethostbyname");
        exit(1);
    }
    printf("gethostbyname: %u %d\n", *(int *)hst->h_addr, hst->h_length);

    // Fill structure
    bcopy(hst->h_addr, &saddr.sin_addr, hst->h_length);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(atoi(port));

    do
    {
        // Connect to other socket(mds)
        if (connect(s, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
        {
            perror("connect hasent work try again");
        }
        else
        {
            isConnected = 1;
            puts("connect done");
        }
    } while (isConnected == 0);

    do
    {

        // read the type of call from socket
        if (receive_int(&type, s) < 0)
        {
            perror("read call type");
            goto restart; //mds is probably down go back to connection
        }
        else
        {
            //type 1 for echo
            if (type == 1)
            {
                // Read (or recv) from socket
                printf("echo call from Mds \n");
                //return the same value 1 for notifing that the Vdr is online ;
                if (send_int(type, s) < 0)
                {
                    perror("write");
                    exit(1);
                }
            }
            //type six for message to store
            if (type == 6)
            {
                //set socret=1 for returning it to the MDS
                socret = 1;
                //if the file is not stored set socret=0
                if (storeMessage(s))
                    socret = 0;
                //write socret in the socket
                if (send_int(socret, s) < 0)
                {
                    perror("send");
                    exit(1);
                }
            }
            //type seven for get ninbox
            if (type == 7)
            {
                char user[MAXLIMIT];
                int len;
                //read the len of the user you are about to recive
                if (receive_int(&len, s) < 0)
                {
                    perror("read");
                    exit(1);
                }
                //read the username to search inbox;
                if (read(s, &user, len + 1) < 0)
                {
                    perror("read");
                    exit(1);
                }
                user[len] = '\0';
                //socret= ninbox is 0 either if the file with ninbox dosen't esist or if ninbox=0;
                socret = getinbox(user);
                //return socret to the socket
                if (send_int(socret, s) < 0)
                {
                    perror("write");
                    exit(1);
                }
            }
            //type 8 for get all the message from a user sending it one to one to avoid huge package
            if (type == 8)
            {
                char user[MAXLIMIT];
                int userlen;
                //read the username to search Audiodata;
                if (receive_int(&userlen, s) < 0)
                {
                    perror("read");
                    return 1;
                }
                if (read(s, &user, userlen + 1) < 0)
                {
                    perror("read");
                    exit(1);
                }
                //return 0 if there is an error and 1 if everything has gone fine       //return 0 if there is an error and 1 if everything has gone fine
                if (readandsendMessages(user, s))
                {
                    perror("cant send messages");
                    exit(1);
                }
            }
            //type 9 is for check if the user was ever register in this vdr;
            if (type == 9)
            {
                char user[MAXLIMIT];
                int len;
                //read the username to search inbox;
                if (receive_int(&len, s) < 0)
                {
                    perror("read");
                    exit(1);
                }
                if (read(s, user, len + 1) < 0)
                {
                    perror("read");
                    exit(1);
                }
                printf("Im chek if %s is here \n", user);
                //socret= return 0 if the file with ninbox dosen't esist 1 else ;
                socret = checkuser(user);
                //return socret to the socket
                if (send_int(socret, s) < 0)
                {
                    perror("write");
                    exit(1);
                }
            }
            //type 10 to delate a message;
            if (type == 10)
            {
                socret = 1;
                PackageData todel;
                if (receive_int(&todel.type, s) < 0)
                {
                    perror("read");
                    return 1;
                }
                if (todel.type != 6)
                {
                    perror("TYPE INCOMPATIBILITY");
                    return 1;
                }

                recive_PackageData(&todel, s);
                //if the file is not stored set socret=0
                if (delateMessage(todel))
                {
                    perror("cant remove the message");
                    socret = 1;
                }
                else
                    socret = 0;
                if (send_int(socret, s) < 0)
                {
                    perror("write return value");
                }
            }
            //type 10 to delate all the messages of a user
            if (type == 12)
            {
                socret = 1;
                char username[MAXLIMIT];
                int len; // the len of the username
                if (receive_int(&len, s) < 0)
                {
                    perror("read");
                    return 1;
                }
                if (read(s, username, len + 1) < 0)
                {
                    perror("write");
                    exit(1);
                }
                username[len] = '\0'; //add termination;

                if (delateallMessages(username))
                {
                    perror("cant remove all  the messages");
                    socret = 1;
                }
                else
                    socret = 0;
                if (send_int(socret, s) < 0)
                {
                    perror("write return value");
                }
            }
        }
    } while (type != 5);
    // Close the vdr socket
    close(s);
}
