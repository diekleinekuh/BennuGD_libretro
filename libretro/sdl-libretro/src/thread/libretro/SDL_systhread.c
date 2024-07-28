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
#include "../SDL_thread_c.h"
#include "../SDL_systhread.h"
#include "rthreads/rthreads.h"


int SDL_SYS_CreateThread(SDL_Thread *thread, void *args)
{
	thread->handle = sthread_create(&SDL_RunThread, args);

	if (!thread->handle)
	{
		SDL_SetError("sthread_create failed");
		return -1;
	}

	return(0);
}

void SDL_SYS_SetupThread(void)
{
}

void* SDL_ThreadID(void)
{
	return (void*)sthread_get_current_thread_id();
}

void SDL_SYS_WaitThread(SDL_Thread *thread)
{
	sthread_join(thread->handle);
}

void SDL_SYS_KillThread(SDL_Thread *thread)
{
	// cannot be safely implemented most of the time and seems unused
}
