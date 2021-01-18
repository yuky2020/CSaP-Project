/**
 * @file util.h
 * @brief Contains AudioDataf structure,relevent audio functions and PackageData Structure
**/
#include <stdint.h>
#include <string.h>
#define MAXLIMIT 20
/**
 * Holds all of the necessary information for building an
 * audio file.
 * @var duration Contains how long the audio file will be in seconds.
 * @var formatType Contains the audio formatting type.
 * @var numberOfChannels Contains the number of audio channels.
 * @var sampleRate Contains the sample rate in Hertz.

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
//AudioData initAudioData(uint32_t sample_rate, uint16_t channels, uint32_t duration);

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
    AudioDataf  message;
    int         hash;
    char        timestamp[MAXLIMIT];
}PackageData; 

