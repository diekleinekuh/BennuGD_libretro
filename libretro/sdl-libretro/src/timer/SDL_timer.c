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

#include "SDL_timer.h"
#include "SDL_timer_c.h"
#include "SDL_mutex.h"
#include "SDL_systimer.h"

/* #define DEBUG_TIMERS */

int SDL_timer_started = 0;
int SDL_timer_running = 0;

/* Data to handle a single periodic alarm */
Uint32 SDL_alarm_interval = 0;
SDL_TimerCallback SDL_alarm_callback;

/* Data used for a thread-based timer */
static int SDL_timer_threaded = 0;

int SDL_TimerInit(void)
{
	int retval;

	retval = 0;
	if ( SDL_timer_started ) {
		SDL_TimerQuit();
	}
	retval = SDL_SYS_TimerInit();
	
	if ( retval == 0 ) {
		SDL_timer_started = 1;
	}
	return(retval);
}

void SDL_TimerQuit(void)
{
	SDL_SYS_TimerQuit();
	SDL_timer_started = 0;
}

