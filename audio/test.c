#include <stdio.h>
#include <stdlib.h>
#include "portaudio.h"
#include <math.h>

static int patestCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    /* Cast data passed through stream to our structure. */
    float *data = (float*)userData; 
    float *out = (float*)outputBuffer;
    unsigned int i;
    (void) outputBuffer; /* Prevent unused variable warning. */
    
    for( i=0; i<framesPerBuffer; i++ )
    {
        *out++ = *data > 0 ? 1.0f : 0.0f;
        /* Generate simple sawtooth phaser that ranges between -1.0 and 1.0. */
		*data+= 0.06f;
        /* When signal reaches top, drop back down. */
        if( *data >= 1.0f ) *data -= 2.0f;
    }
    return 0;
}

int main(void)
{
	float udata;
	PaStream *stream;
	PaError err;
	
	err = Pa_Initialize();
	if(err != paNoError)
	{
		printf("Fuck.");
		return 0;
	}
	
	err = Pa_OpenDefaultStream(&stream, 1, 0, paFloat32, 44100, 256, patestCallback, &udata);
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
