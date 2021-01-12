/**
 * @file record.c
 * @brief Records a FLAC audio file
 */

#include <stdio.h>
#include <stdlib.h>
#include <portaudio.h>
#include <sndfile.h>
#include "audio.h"

/**
 * @fn initAudioData
 * @param sampleRate The sample rate in Hertz in which the audio is to be recorded
 * @param channels The number of channels in which the audio is to be recorded
 * @param duration How long in seconds the recording will be
 * @return a partially initialized structure.
 */

AudioData initAudioData(uint32_t sampleRate, uint16_t channels, uint32_t duration)
{
    AudioData data;
    data.duration = duration;
    data.formatType = 1;
    data.numberOfChannels = channels;
    data.sampleRate = sampleRate;
    data.frameIndex = 0;
    data.maxFrameIndex = sampleRate * duration;
    return data;
}

/**
 * This routine will be called by the PortAudio engine when audio is needed.
 * It may be called at interrupt level on some machines so don't do anything
 * that could mess up the system like calling malloc() or free().
 */
static int recordCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData)
{
    AudioData* data = (AudioData*) userData;
    const float* rptr = (const float*)inputBuffer;
    float* wptr = &data->recordedSamples[data->frameIndex * data->numberOfChannels];
    long framesToCalc;
    int finished;
    unsigned long framesLeft = data->maxFrameIndex - data->frameIndex;

    (void) outputBuffer; /* Prevent unused variable warnings. */
    (void) timeInfo;
    (void) statusFlags;
    (void) userData;

    if(framesLeft < framesPerBuffer)
    {
        framesToCalc = framesLeft;
        finished = paComplete;
    }
    else
    {
        framesToCalc = framesPerBuffer;
        finished = paContinue;
    }

    if(!inputBuffer)
    {
        for(long i = 0; i < framesToCalc; i++)
        {
            *wptr++ = 0.0f;  // left
            if(data->numberOfChannels == 2) *wptr++ = 0.0f;  // right
        }
    }
    else
    {
        for(long i = 0; i < framesToCalc; i++)
        {
            *wptr++ = *rptr++;  // left
            if(data->numberOfChannels == 2) *wptr++ = *rptr++;  // right
        }
    }
    data->frameIndex += framesToCalc;
    return finished;
}

/**
 * @fn recordFLAC
 * @param data An initiallized (with initAudioData()) AudioData structure
 * @param fileName The name of the file in which the recording will be stored
 * @return Success value
 */

int recordFLAC(AudioData data, const char *fileName)
{
    PaStreamParameters inputParameters;
    PaStream* stream;
    int err = 0;
    int numSamples = data.maxFrameIndex * data.numberOfChannels;
    int numBytes = numSamples * sizeof(float );

    data.recordedSamples = malloc(numSamples*numBytes); // From now on, recordedSamples is initialised.
    if(data.recordedSamples==NULL)
    {
        fprintf(stderr, "Could not allocate record array.\n");
        goto done;
    }
    if((err = Pa_Initialize())) goto done;

    inputParameters.device = Pa_GetDefaultInputDevice();
    if (inputParameters.device == paNoDevice)
    {
        fprintf(stderr,"Error: No default input device.\n");
        goto done;
    }
    inputParameters.channelCount = data.numberOfChannels;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(&stream, &inputParameters, NULL, data.sampleRate, paFramesPerBufferUnspecified, paClipOff, recordCallback, &data);
    if(err) goto done;

    if((err = Pa_StartStream(stream))) goto done;
    puts("=== Now recording!! Please speak into the microphone. ===");

    while(1 == (err = Pa_IsStreamActive(stream)))
    {
        Pa_Sleep(1000);
        printf("index = %d\n", data.frameIndex);
    }
    if((err = Pa_CloseStream(stream))) goto done;

    done:
    Pa_Terminate();
    if(err)
    {
        fprintf(stderr, "An error occured while using the portaudio stream: %d\n", err);
        fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
        err = 1;      // Always manually return positive codes.
    }
    else
    {
        SF_INFO sfinfo;
        sfinfo.channels = data.numberOfChannels;
        sfinfo.samplerate = data.sampleRate;
        sfinfo.format = SF_FORMAT_FLAC | SF_FORMAT_PCM_16;
        SNDFILE* outfile = sf_open(fileName, SFM_WRITE, &sfinfo); // open to file
        if (!outfile) return -1;
        long wr = sf_write_float(outfile, data.recordedSamples, numSamples); // write the entire buffer to the file
        if (wr < numSamples) fprintf(stderr, "Only wrote %lu frames, should have written %d", wr, numSamples);
        sf_write_sync(outfile); // force write to disk
        sf_close(outfile); // don't forget to close the file
    }
    free(data.recordedSamples);
    return err;
}