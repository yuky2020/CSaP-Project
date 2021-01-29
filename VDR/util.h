/**
 * @file util.h
 * @brief Contains AudioDataf structure,relevent audio functions and PackageData Structure
**/
#include <stdint.h>
#include <string.h>
#define ADDRESSBOOKLIMIT 200
#define MAXLIMIT    20 //limit for username and Password
#define FRAMESIZE   1024//size of a audio Frame 
#define TIMESTAMPS  26//the size of the string with time stamp;
/**
  * @var frameIndex Contains the current frame to be processed.
 * @var maxFrameIndex Contains the number of frames that the audio file will store.
 * @var recordedSamples Contains the raw PCM audio data.
 */
typedef float SAMPLE;
typedef struct
{
    int          frameIndex;  /* Index into sample array. */
    int          maxFrameIndex;
    SAMPLE      *recordedSamples;
}
        AudioDataf;

//int recordFLAC(AudioData data, const char *fileName);

//Contains data about a user;
typedef struct
{
    int         type; //type is the type of package to be sent 1 for registration 2 for login 
    char        username[MAXLIMIT];
    char        password[MAXLIMIT];
    

}userData; 

AudioDataf record(); //record 5 second of audio use this value because  each AudioData structure in this way is not too big 
int playback(AudioDataf data);// playback audio data structure
AudioDataf recordP();// This implementation of the recorder can record until a key is pressed and auto remove silence from registration 

typedef struct
{
    int         type; //type is the type of package to be sent for a package is 6
    char        from[MAXLIMIT];
    char        to[MAXLIMIT];
    int         size; //size of the message ;
    AudioDataf  message;
    int         hash;
    char        timestamp[TIMESTAMPS];
}PackageData; 
int hashCode(PackageData);//is the hashCode for a package; 

