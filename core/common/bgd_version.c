#include "git.h"
#include <stdio.h>

static char buffer[1024] = {0};

const char* bgd_getversion()
{
    if (*buffer==0)
    {
        snprintf(buffer, sizeof(buffer), "1.0.0-BennuGD_libretro-%s-%s", git_Branch(), git_CommitSHA1() );
    }

    return buffer;
}
