#include "filesystem.h"
#include "files_st.h"
#include "dirs.h"
#include "libretro.h"
#include "streams/file_stream.h"
#include "string/stdstring.h"
#include "filestream_gzip.h"
#include "file/file_path.h"
#include "retro_dirent.h"
#include "retro_miscellaneous.h"
#include "compat/strl.h"
#include <assert.h>
#include <ctype.h>

#define SKIP_STDIO_REDEFINES 1
#include "streams/file_stream_transforms.h"

extern retro_log_printf_t log_cb;

char * content_basename=NULL;
char * bgd_current_dir=NULL;
char * retro_current_dir=NULL;
const char * bgd_dir_root = "/fake_root_dir/";
char * retro_dir_root = NULL;
size_t bgd_dir_root_len = 0;
size_t retro_dir_root_len = 0;

static bool case_insensitive_io = false;


#ifdef _MSC_VER
#   define THREAD_LOCAL __declspec(thread)
#else
#   define THREAD_LOCAL _Thread_local
#endif

#ifndef CASE_INSENSITIVE_FILESYSTEM_EMULATION
#   if defined(_WIN32)
#       define CASE_INSENSITIVE_FILESYSTEM_EMULATION 0
#   else
#       define CASE_INSENSITIVE_FILESYSTEM_EMULATION 1
#   endif
#endif

typedef enum filename_cache_response
{
    FCR_NONE,
    FCR_HIT,
    FCR_MISS
} filename_cache_response_t;

#if CASE_INSENSITIVE_FILESYSTEM_EMULATION
#include <stdlib.h>
#include <string.h>
#include <compat/strl.h>
#include <errno.h>
#include <sys/stat.h>
#include "rthreads/rthreads.h"
#include "array/rhmap.h"

static slock_t* file_map_lock=NULL;

typedef struct file_map
{
    char* real_name;
    bool is_dir;
} file_map_t;

static file_map_t* file_map;


static void create_file_map(const char* root_dir, char * directory_name, char* buffer, size_t buffer_size )
{
    snprintf(buffer, buffer_size, "%s%s", root_dir, directory_name);
    struct RDIR* dir = retro_opendir_include_hidden(buffer, true);
    if (dir)
    {
        while((retro_readdir(dir)))
        {
            const char* entry_name = retro_dirent_get_name(dir);
            if (strcmp(entry_name, "..")==0 || strcmp(entry_name, ".")==0)
            {
                continue;
            }

            int buffer_used = *directory_name ? snprintf(buffer, buffer_size, "%s/%s", directory_name, entry_name) : snprintf(buffer, buffer_size, "%s", entry_name);
            string_to_lower(buffer);

            file_map_t* entry = RHMAP_PTR_STR(file_map, buffer);
            buffer_used = *directory_name ? snprintf(buffer, buffer_size, "%s%s/%s", root_dir, directory_name, entry_name) : snprintf(buffer, buffer_size, "%s%s", root_dir, entry_name);
            entry->real_name = strldup(buffer, buffer_used+1);
            entry->is_dir = retro_dirent_is_dir(dir, NULL);

            if (entry->is_dir)
            {
                create_file_map(root_dir, entry->real_name+strlen(root_dir), buffer, buffer_size);
            }
        }

        retro_closedir(dir);
    }
}

static void destroy_file_map()
{
    for (size_t idx = 0, cap = RHMAP_CAP(file_map); idx != cap; idx++)
    {
        if (RHMAP_KEY(file_map, idx))
        {
            free(file_map[idx].real_name);
        }
    }
    RHMAP_FREE(file_map);
    file_map = NULL;
}

typedef struct directory_entries
{
    file_map_t* filemap;
} directory_entries_t;

directory_entries_t* directory_entries_map = NULL;

static void remove_from_filename_cache(const char* filename)
{
    if (path_is_absolute(filename))
    {
        THREAD_LOCAL static char buffer[PATH_MAX_LENGTH];
        if (strstr(filename, retro_dir_root)==filename)
        {
            snprintf(buffer, PATH_MAX_LENGTH, "%s", filename+retro_dir_root_len);
            string_to_lower(buffer);
            
            slock_lock(file_map_lock);
            assert(RHMAP_HAS_STR(file_map, buffer));
            RHMAP_DEL_STR(file_map, buffer);
            slock_unlock(file_map_lock);
        }
    }
    else
    {
        assert(false);
    }
}

static void add_filename_to_cache(const char* filename)
{
    if (path_is_absolute(filename))
    {
        THREAD_LOCAL static char buffer[PATH_MAX_LENGTH];
        if (strstr(filename, retro_dir_root)==filename)
        {
            snprintf(buffer, PATH_MAX_LENGTH, "%s", filename+retro_dir_root_len);
            string_to_lower(buffer);

            char * real_name = strdup(filename);
            
            slock_lock(file_map_lock);
            assert(!RHMAP_HAS_STR(file_map, buffer));

            file_map_t* entry = RHMAP_PTR_STR(file_map, buffer);
            entry->real_name = real_name;
            slock_unlock(file_map_lock);
        }
    }
    else
    {
        assert(false);
    }
}

static const char* casepath(char const *path, size_t start_offset, bool try_partial_match, filename_cache_response_t* cache_response)
{
    THREAD_LOCAL static char buffer[PATH_MAX_LENGTH]={0};
    const int buffer_used=snprintf(buffer, PATH_MAX_LENGTH, "%s", path+start_offset);
    char* current_buffer_end = buffer+buffer_used;

    string_to_lower(buffer);
    
    slock_lock(file_map_lock);


    while(current_buffer_end>buffer)
    {
        ptrdiff_t idx_entries=RHMAP_IDX_STR(file_map, buffer);
        if (idx_entries>=0)
        {
            int written = snprintf(buffer, PATH_MAX_LENGTH, "%s", file_map[idx_entries].real_name);
            slock_unlock(file_map_lock);

            const char * unchanged_part=path + written;
            if (*unchanged_part)
            {
                snprintf(buffer+written, PATH_MAX_LENGTH-written, "%s", unchanged_part);
                *cache_response = FCR_MISS;
            }
            else
            {
                *cache_response = FCR_HIT;
            }
            
            return buffer;
        }

        if (!try_partial_match)
        {
            break;
        }

        while(current_buffer_end>buffer)
        {
            --current_buffer_end;
            if (*current_buffer_end=='/')
            {
                break;
            }
        }
        *current_buffer_end = '\0';
    }

    slock_unlock(file_map_lock);
    *cache_response = FCR_MISS;
    return path;
}
#else
static void add_filename_to_cache(const char* filename) {}
static void remove_from_filename_cache(const char* filename) {}
#endif


void init_filesystem(const char * content_path, const char * save_dir, struct retro_vfs_interface_info* vfs_info, bool in_case_insensitive_io)
{
    case_insensitive_io = in_case_insensitive_io;

    filestream_vfs_init(vfs_info);
    path_vfs_init(vfs_info);
    dirent_vfs_init(vfs_info);

    content_basename=strdup(path_basename(content_path));

    retro_dir_root=strdup(content_path);
    path_basedir(retro_dir_root);
    retro_dir_root_len=strlen(retro_dir_root);

    retro_current_dir=strdup(retro_dir_root);
    bgd_dir_root_len=strlen(bgd_dir_root);

    bgd_current_dir=strdup(bgd_dir_root);

#if CASE_INSENSITIVE_FILESYSTEM_EMULATION
    if (case_insensitive_io)
    {
        file_map_lock = slock_new();
        char buffer[4096];
        size_t buffer_size = sizeof(buffer)/sizeof(buffer[0]);
        create_file_map(retro_dir_root, "", buffer, buffer_size);
    }
#endif
}

void cleanup_filesystem()
{
    free(content_basename);
    free(bgd_current_dir);
    free(retro_dir_root);
    free(retro_current_dir);

#if CASE_INSENSITIVE_FILESYSTEM_EMULATION
    destroy_file_map();    
    slock_free(file_map_lock);
#endif
}

char* get_content_basename()
{
    return content_basename;
}

const char* resolve_bgd_path(const char * dir)
{
    if (path_is_absolute(dir))
    {
        return dir;
    }

    THREAD_LOCAL static char buffer[PATH_MAX_LENGTH];

    if (dir[0]=='.' && dir[1]=='/')
    {
        dir+=2;
    }
    
    snprintf(buffer, PATH_MAX_LENGTH, "%s%s", bgd_current_dir, dir);
    return buffer;
}

const char* to_retro_path(const char * dir, bool try_partial_match, filename_cache_response_t* cache_response)
{
    THREAD_LOCAL static char buffer[PATH_MAX_LENGTH];

    if (path_is_absolute(dir))
    {
        // Check if the passed directory is relative to the fake root directory
        if (strstr(dir, bgd_dir_root)==dir)
        {
            snprintf(buffer, PATH_MAX_LENGTH, "%s%s", retro_dir_root, dir+bgd_dir_root_len);

            #if CASE_INSENSITIVE_FILESYSTEM_EMULATION
            if (case_insensitive_io)
            {
                string_replace_all_chars(buffer, '\\','/');
                return casepath(buffer, retro_dir_root_len, try_partial_match, cache_response);
            }
            #endif

            *cache_response = FCR_NONE;
            return buffer;
        }
    }

    assert(false);
    *cache_response = FCR_NONE;
    return dir;
}

// bgd filesystem forwarded to libretro
bool can_create_new_file(const char * mode)
{
    // expected modes are "rb0", "r+b0", "wb0", "rb", "wb6", not seen anything else in the codebase
    if (*mode=='w')
    {
        return true;
    }

    return false;
}

RFILE * fopen_libretro ( const char * filename, const char * mode )
{
    filename_cache_response_t cache_response;

    const bool mode_can_create_new_file = can_create_new_file(mode);    

    const char* retro_filename = to_retro_path(resolve_bgd_path(filename), mode_can_create_new_file, &cache_response);
    assert(retro_filename);

    // don't even try to open if the cache says no
    if (cache_response==FCR_MISS && mode_can_create_new_file==false)
    {
        return NULL;
    }

    RFILE* rfile = rfopen(retro_filename, mode);

    if (rfile && cache_response==FCR_MISS)
    {
        add_filename_to_cache(retro_filename);
    }

    if (rfile)
    {
        log_cb(RETRO_LOG_DEBUG, "fopen_libretro: Opened file %s(%s)\n", filename, retro_filename);
    }
    else
    {
        log_cb(RETRO_LOG_INFO, "fopen_libretro: Could not find file %s(%s)\n", filename, retro_filename);
    }

    return rfile;
}

int fclose_libretro( RFILE* file)
{
    return rfclose(file);
}

int feof_libretro(RFILE *stream)
{
    return rfeof(stream);
}

int fflush_libretro(RFILE *stream)
{
    return rfflush(stream);
}

size_t fread_libretro(void *ptr, size_t size, size_t nmemb, RFILE *stream)
{
    return rfread(ptr, size, nmemb, stream);
}

int fseek_libretro(RFILE *stream, long int offset, int whence)
{
    return rfseek(stream, offset, whence);
}

long int ftell_libretro(RFILE *stream)
{
    return rftell(stream);
}

size_t fwrite_libretro(const void *ptr, size_t size, size_t nmemb, RFILE *stream)
{
    return rfwrite(ptr, size, nmemb, stream);
}

int remove_libretro(const char *filename)
{
    filename_cache_response_t cache_response;
    const char* resolved_filename = to_retro_path(resolve_bgd_path(filename), false, &cache_response);
    assert(resolved_filename);
    if (filestream_delete(resolved_filename)==0)
    {
        if (cache_response==FCR_HIT)
        {
            remove_from_filename_cache(resolved_filename);
        }

        return 0;
    }

    return -1;
}

int rename_libretro(const char *old_filename, const char *new_filename)
{
    filename_cache_response_t cache_response_old, cache_response_new;
    const char * retro_old = to_retro_path(resolve_bgd_path(old_filename), false, &cache_response_old);
    const char * retro_new = to_retro_path(resolve_bgd_path(new_filename), true, &cache_response_new);

    assert(retro_old);
    assert(retro_new);

    if (filestream_rename( old_filename, new_filename)==0)
    {
        if (cache_response_old==FCR_HIT)
        {
            remove_from_filename_cache(retro_old);
        }

        if (cache_response_new==FCR_MISS)
        {
            add_filename_to_cache(retro_new);
        }

        return 0;
    }

    return -1;
}

char *fgets_libretro(char *str, int n, RFILE *stream)
{
    return rfgets(str, n, stream);
}

const char * get_current_dir_libretro( )
{
    return bgd_current_dir;
}


int chdir_libretro( const char * dir)
{
    filename_cache_response_t dummy;
    const char* bgd_dir = resolve_bgd_path(dir);
    const char * retro_dir = to_retro_path(bgd_dir, false, &dummy);

    assert(retro_dir);

    if (path_is_directory(retro_dir))
    {
        free(retro_current_dir);
        retro_current_dir=strdup(retro_dir);
        free(bgd_current_dir);
        bgd_current_dir=strdup(bgd_dir);
        return 0;
    }

    return -1;
}

int mkdir_libretro( const char * dir )
{
    filename_cache_response_t cache_response;
    const char * retro_dir = to_retro_path(resolve_bgd_path(dir), true, &cache_response);
    assert(retro_dir);

    if (path_mkdir( retro_dir ) == 0)
    {
        if (cache_response == FCR_MISS)
        {
            add_filename_to_cache(retro_dir);
        }
        return 0;
    }

    return -1;
}

int rmdir_libretro( const char * dir)
{    
    return remove_libretro(dir);
}

int unlink_libretro(const char * filename)
{
    return remove_libretro(filename);
}

int diropen_libretro(__DIR_ST * hDir)
{    
    char buffer[PATH_MAX_LENGTH];
    strlcpy(buffer, resolve_bgd_path(hDir->path), PATH_MAX_LENGTH);

    char* last_slash = find_last_slash(buffer);
    if (!last_slash)
    {
        return 0;
    }
        
    //char slash_char = *last_slash;
    *last_slash = 0;

    filename_cache_response_t dummy;
    const char * retro_dir = to_retro_path(buffer, false, &dummy);

    hDir->rdir = retro_opendir_include_hidden(retro_dir, true);
    if (!hDir->rdir)
    {
        return 0;
    }

    char * pattern_start = last_slash+1;
    hDir->pattern = strdup(pattern_start);
    *pattern_start = 0;

    hDir->dir = strdup(buffer);

    // *last_slash = slash_char;
    // hDir->dir = strdup(buffer);
    
    return 1;
}

void dirclose_libretro(__DIR_ST * hDir)
{
    retro_closedir(hDir->rdir);
    free(hDir->dir);
    free(hDir->pattern);
}

int dirread_libretro(__DIR_ST* hdir)
{
    while (retro_readdir(hdir->rdir))
    {
        const char * filename = retro_dirent_get_name(hdir->rdir);
        
        //if (mattern_matches(filename))
        {
            memset(&hdir->info, 0, sizeof(hdir->info));
            strlcpy(hdir->info.filename, filename, __MAX_PATH);
            fill_pathname_join_special(hdir->info.fullpath, hdir->dir, filename, __MAX_PATH);
            if (retro_dirent_is_dir(hdir->rdir, NULL))
            {
                hdir->info.attributes |= DIR_FI_ATTR_DIRECTORY;
            }

            filename_cache_response_t dummy;
            const char * retro_filepath_full = to_retro_path(hdir->info.fullpath, false, &dummy);
            hdir->info.size = path_get_size(retro_filepath_full);

            return true;
        }
    }

    return false;
}
