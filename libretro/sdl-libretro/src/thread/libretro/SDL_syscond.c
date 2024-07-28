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


#include "SDL_thread.h"
#include "SDL_sysmutex_c.h"

#include "rthreads/rthreads.h"

typedef struct SDL_cond SDL_cond;

/* Create a condition variable */
SDL_cond * SDL_CreateCond(void)
{
	return (SDL_cond *)scond_new();
}

/* Destroy a condition variable */
void SDL_DestroyCond(SDL_cond *cond)
{
	if ( cond )
	{
		scond_free((scond_t*)cond);
	}
}

/* Restart one of the threads that are waiting on the condition variable */
int SDL_CondSignal(SDL_cond *cond)
{
	if ( ! cond ) {
		SDL_SetError("Passed a NULL condition variable");
		return -1;
	}

	scond_signal((scond_t*)cond);

	return 0;
}

/* Restart all threads that are waiting on the condition variable */
int SDL_CondBroadcast(SDL_cond *cond)
{
	if ( ! cond ) {
		SDL_SetError("Passed a NULL condition variable");
		return -1;
	}

	if ( scond_broadcast((scond_t*)cond) != 0 ) {
		SDL_SetError("scond_broadcast() failed");
		return -1;
	}
	return 0;
}

int SDL_CondWaitTimeout(SDL_cond *cond, SDL_mutex *mutex, Uint32 ms)
{
	if ( ! cond ) {
		SDL_SetError("Passed a NULL condition variable");
		return -1;
	}

	if (!mutex)
	{
		SDL_SetError("Passed a NULL mutex variable");
		return -1;		
	}

	if (!scond_wait_timeout((scond_t*)cond, (slock_t*)mutex, ms*1000))
	{
		return SDL_MUTEX_TIMEDOUT;
	}

	return 0;
}

/* Wait on the condition variable, unlocking the provided mutex.
   The mutex must be locked before entering this function!
 */
int SDL_CondWait(SDL_cond *cond, SDL_mutex *mutex)
{
	int retval;

	if ( ! cond ) {
		SDL_SetError("Passed a NULL condition variable");
		return -1;
	}

	if (!mutex)
	{
		SDL_SetError("Passed a NULL mutex variable");
		return -1;		
	}	

	scond_wait((scond_t*)cond, (slock_t*)mutex);
	return 0;
}
