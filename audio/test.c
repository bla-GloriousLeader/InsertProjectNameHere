#include <stdio.h>
#include <stdlib.h>
#include "portaudio.h"
#include <math.h>

typedef struct{
	int max_size;
	int current_size;
	int playback_current_size;
	float *data;
} Buffer;

static int patestCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    /* Cast data passed through stream to our structure. */
    float *in = (float*) inputBuffer;
    float *out = (float*) outputBuffer;
    unsigned int i;
    Buffer *buf = (Buffer*) userData;
    
    for( i=0; i<framesPerBuffer; i++ )
    {
        if(buf->current_size < buf->max_size)
        {
			buf->data[buf->current_size] = in[i];
			buf->current_size++;
			
		} else if(buf->playback_current_size < buf->max_size)
		{
			out[i] = (-buf->data[buf->playback_current_size]);
			buf->playback_current_size++;
		} else
		{
			out[i] = 0;
		}
    }
    
    return 0;
}

int main(void)
{
	Buffer udata;
	
	float adata[65536 * 4];
	
	PaStream *stream;
	PaError err;
	
	err = Pa_Initialize();
	if(err != paNoError)
	{
		printf("Fuck.");
		return 0;
	}
	
	udata.max_size = 65536 * 4;
	udata.current_size = 0;
	udata.playback_current_size = 0;
	udata.data = adata;
	
	err = Pa_OpenDefaultStream(&stream, 1, 1, paFloat32, 44100, 256, patestCallback, &udata);
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
	
	Pa_Sleep(12000);
	
	Pa_StopStream(stream);
	
	Pa_CloseStream(stream);
	
	Pa_Terminate();
	
	return 0;
}
