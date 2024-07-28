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
#include "rthreads/rthreads.h"

SDL_mutex *SDL_CreateMutex (void)
{
	return (SDL_mutex*)slock_new();
}

void SDL_DestroyMutex(SDL_mutex *mutex)
{
	if ( mutex )
	{
		slock_free((slock_t*)mutex);
	}
}

/* Lock the mutex */
int SDL_mutexP(SDL_mutex *mutex)
{
	if ( mutex == NULL ) {
		SDL_SetError("Passed a NULL mutex");
		return -1;
	}

	slock_lock((slock_t*)mutex);
	return 0;
}

int SDL_mutexV(SDL_mutex *mutex)
{
	if ( mutex == NULL ) {
		SDL_SetError("Passed a NULL mutex");
		return -1;
	}

	slock_unlock((slock_t*)mutex);

	return 0;
}
