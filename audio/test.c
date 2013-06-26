#include <stdio.h>
#include <stdlib.h>
#include "portaudio.h"
#include <math.h>
#include "opus.h"

typedef struct{
	int max_size;
	int current_size;
	float *data;
} Buffer;

OpusEncoder *enc;
OpusDecoder *dec;

static int patestCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    /* Cast data passed through stream to our structure. */
    float *in = (float*) inputBuffer;
    float *out = (float*) outputBuffer;
    Buffer *buf = (Buffer*) userData;
    
    buf->current_size = (int) opus_encode_float(enc, in, 256, (unsigned char*) buf->data, buf->max_size * sizeof(float));
    
    buf->current_size = opus_decode_float(dec, (unsigned char*) buf->data, (opus_int32) buf->current_size, out, 256, 0);
     
    return 0;
}

int main(void)
{
	Buffer udata;
	
	float adata[512];
	
	PaStream *stream;
	PaError err;
	
	int opuserror;
	
	err = Pa_Initialize();
	if(err != paNoError)
	{
		printf("Error: Failed to initialize PortAudio");
		return 0;
	}
	
	udata.max_size = 512;
	udata.current_size = 0;
	udata.data = adata;
	
	//Start up an opus encoder, 1 channel
	enc = opus_encoder_create(48000, 1, OPUS_APPLICATION_AUDIO, &opuserror);
	if(opuserror != OPUS_OK)
	{
		printf("Error: Failed to create an opus encoder.");
		return 0;
	}
	
	//Start up an opus decoder, 1 channel
	dec = opus_decoder_create(48000, 1, &opuserror);
	if(opuserror != OPUS_OK)
	{
		printf("Error: Failed to create an opus decoder.");
		return 0;
	}
	
	
	//Start up a PortAudio stream, 1-channel, both input and output
	//Using a sample rate of 48000 here because opus doesn't seem to support 44100
	err = Pa_OpenDefaultStream(&stream, 1, 1, paFloat32, 48000, 256, patestCallback, &udata);
	if(err != paNoError)
	{
		printf("Error: Failed to open an audio stream.");
		return 0;
	}
	
	err = Pa_StartStream(stream);
	if(err != paNoError)
	{
		printf("Error: Failed to start the audio stream.");
		return 0;
	}
	
	Pa_Sleep(12000);
	
	err = Pa_StopStream(stream);
	if(err != paNoError)
	{
		printf("Error: Failed to stop the audio stream.  This is bad...");
		return 0;
	}
	
	err = Pa_CloseStream(stream);
	if(err != paNoError)
	{
		printf("Error: Failed to close the audio stream.  This might cause concern.");
		return 0;
	}
	
	//I could do error checking here, but we're about to return anyways...
	Pa_Terminate();
	
	return 0;
}
