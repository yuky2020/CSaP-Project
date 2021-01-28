/**
 * @file record.c
 * @brief Records an AudioData recording from some times using poisix semaphore and child process to stop when user want 
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "portaudio.h"
#include <sndfile.h>
#include "util.h"
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#define SEM_NAME "/semaphore_12345678910"
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define INITIAL_VALUE 1
#define FRAMES_PER_BUFFER 1024


typedef struct
{
    uint16_t formatType;
    uint8_t numberOfChannels;
    uint32_t sampleRate;
    size_t size;
    float *recordedSamples;
} AudioData;

typedef struct
{
    float *snippet;
    size_t size;
} AudioSnippet;


/**
 * @fn initAudioData
 * @param sampleRate The sample rate in Hertz in which the audio is to be recorded
 * @param channels The number of channels in which the audio is to be recorded
 * @param duration How long in seconds the recording will be
 * @return a partially initialized structure.
 */


AudioData initAudioData(uint32_t sampleRate, uint16_t channels, int type)
{
    AudioData data;
    data.formatType = type;
    data.numberOfChannels = channels;
    data.sampleRate = sampleRate;
    data.size = 0;
    data.recordedSamples = NULL;
    return data;
}

float avg(float *data, size_t length)
{
    float sum = 0;
    for (size_t i = 0; i < length; i++)
    {
        sum += fabs(*(data + i));
    }
    return (float) sum / length;
}

/**
 * @fn storeFLAC was more a test for how to store data  in the end is not used in the project because i opted to store the full package  
 * @param data An initiallized (with initAudioData()) AudioData structure alredy filled 
 * @param fileName The name of the file in which the recording will be stored
 * @return Success value
 */

long storeFLAC(AudioData *data, const char *fileName)
{

    uint8_t err = SF_ERR_NO_ERROR;
    SF_INFO sfinfo =
    {
        .channels = data->numberOfChannels,
        .samplerate = data->sampleRate,
        .format = SF_FORMAT_FLAC | SF_FORMAT_PCM_16
    };

    SNDFILE *outfile = sf_open(fileName, SFM_WRITE, &sfinfo);
    if (!outfile) return -1;

    // Write the entire buffer to the file
    long wr = sf_writef_float(outfile, data->recordedSamples, data->size / sizeof(float));
    err = data->size - wr;

    // Force write to disk and close file
    sf_write_sync(outfile);
    sf_close(outfile);
    puts("Wrote to file!!!!");
    return err;
}





AudioDataf recordP(void)
{
   //using poisix semphore(a binary semphore) is more easy to menage then a System 5 one
   sem_t *sem1;
   // create, initialize semaphore
    sem1 = sem_open(SEM_NAME, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);
    
    if (sem1 == SEM_FAILED) {
        perror("sem_open(3) error");
        exit(EXIT_FAILURE);
    }
    //port audio inizialization 
    PaError err = paNoError;
    if((err = Pa_Initialize())) goto done;
    const PaDeviceInfo *info = Pa_GetDeviceInfo(Pa_GetDefaultInputDevice());
    AudioData data = initAudioData(44100,2, paFloat32);
    AudioSnippet sampleBlock =
    {
        .snippet = NULL,
        .size = FRAMES_PER_BUFFER * sizeof(float) * data.numberOfChannels
    };
    PaStream *stream = NULL;
    sampleBlock.snippet = malloc(sampleBlock.size);
    time_t talking = 0;
    time_t silence = 0;
    PaStreamParameters inputParameters =
    {
        .device = Pa_GetDefaultInputDevice(),
        .channelCount = data.numberOfChannels,
        .sampleFormat = data.formatType,
        .suggestedLatency = info->defaultHighInputLatency,
        .hostApiSpecificStreamInfo = NULL
    };

    if((err = Pa_OpenStream(&stream, &inputParameters, NULL, data.sampleRate, FRAMES_PER_BUFFER, paClipOff, NULL, NULL))) goto done;
    if((err = Pa_StartStream(stream))) goto done;
    int i=0;
    printf("=======================================================\n");
    printf("inizio registrazione premi e ed enter quando hai finito\n");
    printf("=======================================================\n");
    
   switch(fork()) {
        case -1:
            perror("fork");  /* something went wrong */
            exit(1);         /* parent exits */

        case 0:
            //open semaphore in the child process
            { sem_t *sem1;
            sem1= sem_open(SEM_NAME, O_RDWR);
            if (sem1 == SEM_FAILED) {
            perror("sem_open(3) failed");
            exit(EXIT_FAILURE);
            }
            char ef;//carattere di chiusura
            sleep(3);//time for start registration from child
            getchar();
            
            //wait for a key form tty
            sem_wait(sem1); //lock the semaphore so the child know that has to stop working
            if (sem_close(sem1) < 0)
            perror("sem_close(3) failed");
            exit(0);}

        default:
             while(!sem_trywait(sem1))// check if the binary semaphore is locked (so if the parent as locked you stop the registration) then lockit
             {  
               
               //sem_wait(sem1); // Lock the semaphore
               err = Pa_ReadStream(stream, sampleBlock.snippet, FRAMES_PER_BUFFER);
               if (err) goto done;
               else if(avg(sampleBlock.snippet, FRAMES_PER_BUFFER) > 0.000550) // talking,take only the part of audio with talking signal 
                     {
                       //  printf("You're talking! %d\n", i); for test use only
                        i++;
                        time(&talking);
                        data.recordedSamples = realloc(data.recordedSamples, sampleBlock.size * i);
                        data.size = sampleBlock.size * i;
                        if (data.recordedSamples) memcpy((char*)data.recordedSamples + ((i - 1) * sampleBlock.size), sampleBlock.snippet, sampleBlock.size);
                        else
                           {
                           free(data.recordedSamples);
                           data.recordedSamples = NULL;
                           data.size = 0;
                           }
                     }
                sem_post(sem1); 

            }
        if (sem_unlink(SEM_NAME) < 0) //unlink the semaphore
        perror("sem_unlink(3) failed");   
         sem_destroy(sem1);//destroy the semaphore after usage good pratice

    }

          
   AudioDataf toplayback;
   toplayback.frameIndex=0;
   toplayback.maxFrameIndex=i*1024;
   toplayback.recordedSamples= data.recordedSamples;
   //here for test : playback registered data 
   //playback(toplayback);
done:
    free(sampleBlock.snippet);
    Pa_Terminate();
    if (err!=0)
    perror("err");


    return toplayback;
}
