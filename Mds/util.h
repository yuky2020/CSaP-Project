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
    int         type; //type is the type of package to be sent 1 for registration 2 for login 
    char        username[MAXLIMIT];
    char        password[MAXLIMIT];
    

}userData; 

