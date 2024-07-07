#pragma once

#include <stdio.h>
#include "file/file_path.h"

void init_filesystem(const char * initial_directory, const char * save_dir, struct retro_vfs_interface_info* vfs_info);
void cleanup_filesystem();

char * get_content_basename();

