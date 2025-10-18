#pragma once

#include <stdio.h>
#include "file/file_path.h"

void init_filesystem(const char * initial_directory, const char * save_dir, struct retro_vfs_interface_info* vfs_info);
void cleanup_filesystem();

char * get_content_basename();

struct RFILE * fopen_libretro ( const char * filename, const char * mode );
int fclose_libretro ( struct RFILE * stream );
size_t fread_libretro(void *ptr, size_t size, size_t nmemb, struct RFILE *stream);
int fseek_libretro(struct RFILE *stream, long int offset, int whence);
size_t fwrite_libretro(const void *ptr, size_t size, size_t nmemb, struct RFILE *stream);
