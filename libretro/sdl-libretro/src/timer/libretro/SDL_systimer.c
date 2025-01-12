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
#include "SDL_stdinc.h"

#ifdef SDL_TIMER_LIBRETRO

extern uint64_t retro_get_microseconds();

#include <stdio.h>
//#include <sys/time.h>
//#include <signal.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <string.h>
#include <errno.h>

#include "SDL_timer.h"
#include "../SDL_timer_c.h"

static uint64_t start;

void SDL_StartTicks(void)
{
	start = retro_get_microseconds();
}

Uint32 SDL_GetTicks (void)
{
	return (Uint32)(retro_get_microseconds() - start)/1000;
}

void SDL_Delay (Uint32 ms)
{
#ifdef _WIN32
	Sleep(ms);
#else
	usleep(ms*1000);
#endif
}

/* This is only called if the event thread is not running */
int SDL_SYS_TimerInit(void)
{
	return(0);
}

void SDL_SYS_TimerQuit(void)
{
	return;
}

int SDL_SYS_StartTimer(void)
{
	SDL_SetError("Internal logic error: Should not be used");
	return(-1);
}

void SDL_SYS_StopTimer(void)
{
	SDL_SetError("Internal logic error: Should not be used");
	return;
}

#endif