/**
 * @file util.h
 * @brief Contains AudioData structure,relevent  functions and  userData Structure
**/
#include <stdint.h>
#include <string.h>
#define MAXLIMIT 20


typedef float SAMPLE;
typedef struct
{
    int          frameIndex;  /* Index into sample array. */
    int          maxFrameIndex;
    SAMPLE      *recordedSamples;
}AudioData;


typedef struct
{
    int         type;// type of the package for this is always 6
    char        from[MAXLIMIT] //user name of the sender
    char        to[MAXLIMIT]  //user name of the reciver
    
    AudioData   message; //the audio message
    int         hash;// hash for controll if the data is corrupted  

}MessageData;



