/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2012 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"

/* Allow access to a raw mixing buffer */

#include "SDL.h"
#include "SDL_audio_c.h"
#include "SDL_audiomem.h"
#include "SDL_sysaudio.h"

SDL_AudioDevice *current_audio = NULL;

static SDL_AudioDevice audio;

void sdl_libretro_init_audio()
{
	audio.mixer_lock = SDL_CreateMutex();	
}

void sdl_libretro_cleanup_audio()
{
	if (audio.mixer_lock)
	{
		SDL_DestroyMutex(audio.mixer_lock);
		audio.mixer_lock=NULL;
	}
}

int sdl_libretro_get_sample_rate()
{
	int freq = 0;
	SDL_LockAudio();
	if (current_audio && current_audio->opened)
	{
		freq = current_audio->spec.freq;
	}
	SDL_UnlockAudio();

	return freq;
}

/* Various local functions */
int SDL_AudioInit(const char *driver_name);
void SDL_AudioQuit(void);


void sdl_libretro_runaudio(void* mixbuf, size_t mixbuf_size)
{	
	SDL_memset(mixbuf, 0, mixbuf_size);
	SDL_LockAudio();
	if (current_audio)
	{
		if  ( current_audio->opened ) {
			if ( ! current_audio->paused ) {
				(*current_audio->spec.callback)(current_audio->spec.userdata, mixbuf, mixbuf_size);
			}
		}
	}

	SDL_UnlockAudio();
}

int SDL_AudioInit(const char *driver_name)
{
	SDL_LockAudio();
	current_audio=&audio;
	current_audio->opened = 0;
	SDL_UnlockAudio();
	return 0;
}

char *SDL_AudioDriverName(char *namebuf, int maxlen)
{	
	if (current_audio)
	{
		SDL_strlcpy(namebuf, "libretro", maxlen);
		return(namebuf);
	}

	return NULL;
}

int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained)
{	
	if ( desired->freq == 0 ) {
		/* Pick some default audio frequency */
		desired->freq = 22050;
	}

	if ( desired->format == 0 ) {
		/* Pick some default audio format */
		desired->format = AUDIO_S16;
	}

	if ( desired->channels == 0 ) {
		/* Pick a default number of channels */
		desired->channels = 2;
	}

	if ( desired->callback == NULL ) {
		SDL_SetError("SDL_OpenAudio() passed a NULL callback");
		return(-1);
	}

	desired->samples = 1024;

	if (!obtained)
	{
		if (desired->format!=AUDIO_S16SYS)
		{
			SDL_SetError("SDL_OpenAudio() only AUDIO_S16 is supported in libretro implementation");
			return(-1);
		}

		if (desired->channels != 2)	
		{
			SDL_SetError("SDL_OpenAudio() only stereo supported in libretro implementation");
			return(-1);
		}
	}

	/* Calculate the silence and size of the audio specification */
	SDL_CalculateAudioSpec(desired);


	SDL_LockAudio();

	if (current_audio->opened) {
		SDL_SetError("Audio device is already opened");
		SDL_UnlockAudio();
		return(-1);
	}

	/* Open the audio subsystem */
	SDL_memcpy(&current_audio->spec, desired, sizeof(current_audio->spec));
	current_audio->convert.needed = 0;
	current_audio->paused = SDL_TRUE;

	// This is what libretro expects
	current_audio->spec.channels = 2;
	current_audio->spec.format = AUDIO_S16SYS;	
	current_audio->spec.freq = desired->freq;
	current_audio->spec.samples = 1024;
	SDL_CalculateAudioSpec(&current_audio->spec);
	
	if ( obtained != NULL )
	{
		SDL_memcpy(obtained, &current_audio->spec, sizeof(current_audio->spec));
	}

	current_audio->opened = 1;
	SDL_UnlockAudio();

	return(0);
}

SDL_audiostatus SDL_GetAudioStatus(void)
{
	SDL_audiostatus status = SDL_AUDIO_STOPPED;
	
	SDL_LockAudio();
	if ( current_audio && current_audio->opened) {
		if ( current_audio->paused ) {
			status = SDL_AUDIO_PAUSED;
		} else {
			status = SDL_AUDIO_PLAYING;
		}
	}
	SDL_UnlockAudio();
	return(status);
}

void SDL_PauseAudio (int pause_on)
{
	SDL_LockAudio();
	if ( current_audio && current_audio->opened ) {
		current_audio->paused = pause_on;
	}
	SDL_UnlockAudio();
}

void SDL_LockAudio (void)
{
	if (audio.mixer_lock)
	{
		SDL_mutexP(audio.mixer_lock);
	}
}

void SDL_UnlockAudio (void)
{
	if (audio.mixer_lock)
	{
		SDL_mutexV(audio.mixer_lock);
	}	
}

void SDL_CloseAudio (void)
{
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void SDL_AudioQuit(void)
{
	SDL_LockAudio();
	if (current_audio)
	{
		if (current_audio->convert.buf!=NULL)
		{
			SDL_FreeAudioMem(current_audio->convert.buf);
			current_audio->convert.buf=NULL;
		}

		current_audio->opened = 0;
		
		current_audio = NULL;
	}
	SDL_UnlockAudio();
}


void SDL_CalculateAudioSpec(SDL_AudioSpec *spec)
{
	switch (spec->format) {
		case AUDIO_U8:
			spec->silence = 0x80;
			break;
		default:
			spec->silence = 0x00;
			break;
	}
	spec->size = (spec->format&0xFF)/8;
	spec->size *= spec->channels;
	spec->size *= spec->samples;
}

void SDL_Audio_SetCaption(const char *caption)
{
}
