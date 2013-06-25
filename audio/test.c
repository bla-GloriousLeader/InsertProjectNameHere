#include <stdio.h>
#include <stdlib.h>
#include "portaudio.h"
#include <math.h>

typedef struct
{
    float left_phase;
    float right_phase;
}paTestData;
/* This routine will be called by the PortAudio engine when audio is needed.
   It may called at interrupt level on some machines so don't do anything
   that could mess up the system like calling malloc() or free().
*/ 
static int patestCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    /* Cast data passed through stream to our structure. */
    paTestData *data = (paTestData*)userData; 
    float *out = (float*)outputBuffer;
    unsigned int i;
    (void) inputBuffer; /* Prevent unused variable warning. */
    
    for( i=0; i<framesPerBuffer; i++ )
    {
        *out++ = data->left_phase > 0 ? 1.0f : 0.0f;  /* left */
        *out++ = data->right_phase > 0 ? 1.0f : 0.0f;  /* right */
        /* Generate simple sawtooth phaser that ranges between -1.0 and 1.0. */
        data->left_phase += 0.006f;
        /* When signal reaches top, drop back down. */
        if( data->left_phase >= 1.0f ) data->left_phase -= 2.0f;
        /* higher pitch so we can distinguish left and right. */
        data->right_phase += 0.01f;
        if( data->right_phase >= 1.0f ) data->right_phase -= 2.0f;
    }
    return 0;
}

int main(void)
{
	paTestData udata;
	PaStream *stream;
	PaError err;
	
	err = Pa_Initialize();
	if(err != paNoError)
	{
		printf("Fuck.");
		return 0;
	}
	
	err = Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, 44100, 256, patestCallback, &udata);
	if(err != paNoError)
	{
		printf("SHIT!");
		return 0;
	}
	
	err = Pa_StartStream(stream);
	if(err != paNoError)
	{
		printf("God DAMMIT!");
		return 0;
	}
	
	Pa_Sleep(2000);
	
	Pa_StopStream(stream);
	
	Pa_CloseStream(stream);
	
	Pa_Terminate();
	
	return 0;
}
