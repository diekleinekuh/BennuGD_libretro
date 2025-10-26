#include "filesystem.h"
#include "files_st.h"
#include "dirs.h"
#include "libretro.h"
#include "streams/file_stream.h"
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

char * retro_save_dir=NULL;

extern char* retro_save_directory;
extern  size_t retro_save_directory_len;

static bool case_insensitive_io = false;


#ifdef _MSC_VER
#define THREAD_LOCAL __declspec(thread)
#else
#define THREAD_LOCAL _Thread_local
#endif

#if !defined(_WIN32)
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include "rthreads/rthreads.h"
#include "array/rhmap.h"

static slock_t* directory_entries_map_lock=NULL;

typedef struct file_map
{
    char* real_name;
} file_map_t;

typedef struct directory_entries
{
    file_map_t* filemap;
} directory_entries_t;

directory_entries_t* directory_entries_map = NULL;


static bool fix_directory_entry(const char * directory_name, char* file_name)
{
    // file info is stored with lowercae filename as key.
    for (char* c=file_name; *c; ++c)
    {
        *c=tolower(*c);
    }

    //lookup list of files
    slock_lock(directory_entries_map_lock);
    ptrdiff_t idx_entries=RHMAP_IDX_STR(directory_entries_map, directory_name);
    directory_entries_t* entries;
    if (idx_entries<0)
    {
        entries = RHMAP_PTR_STR(directory_entries_map, directory_name);
        assert(entries);

        entries->filemap = NULL;
        // Populate filemap
        struct RDIR* dir = retro_opendir_include_hidden(directory_name, true);
        if (dir)
        {
            while((retro_readdir(dir)))
            {
                const char* filename = retro_dirent_get_name(dir);
                char* lower_filename=strdup(filename);
                for (char* c=lower_filename; *c; ++c)
                {
                    *c=tolower(*c);
                }
                file_map_t new_entry;
                new_entry.real_name=strdup(filename);
                RHMAP_SET_STR(entries->filemap, lower_filename, new_entry);
            }

            retro_closedir(dir);         
        }
        //RHMAP_SET_STR(directory_entries_map, directory_name, entries);
    }
    else
    {
        entries=directory_entries_map+idx_entries;
    }

    file_map_t found_entry = RHMAP_GET_STR(entries->filemap, file_name);
    slock_unlock(directory_entries_map_lock);

    if (found_entry.real_name)
    {
        while(*file_name)
        {
            assert(*found_entry.real_name);
            *file_name = *found_entry.real_name;
            ++file_name;
            ++found_entry.real_name;
        }

        return true;
    }

    return false;
}

static const char* casepath(char const *path, size_t start_offset)
{
    if (!case_insensitive_io)
    {
        return path;
    }

    if (filestream_exists(path))
    {
        return path;
    }

    _Thread_local static char buffer[PATH_MAX_LENGTH]={};

    size_t l = strlen(path);
    if (start_offset>l || l>=sizeof(buffer) || start_offset==0)
    {
        assert(false);
        return path;
    }

    strncpy(buffer, path, l+1);
    
    char * filename = buffer + start_offset;
    
    while(filename<buffer+l)
    {
        char* directory_end = filename - 1;
        *directory_end=0;
        // search for the next directory separator
        char * filename_end = filename;
        while(*filename_end && *filename_end!='/' && *filename_end!='\\')
        {
            ++filename_end;
        }
        *filename_end=0; // clear directory seperator

        if (!fix_directory_entry(buffer, filename))
        {
            return path;
        }

        // copy separator back from source
        *directory_end = path[directory_end-buffer];
        filename = filename_end+1;      
    }

    return buffer;
}
#else
static const char* casepath(char const *path, size_t start_offset)
{
    return path;
}
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

    
    if (save_dir)
    {
        size_t save_dir_length = strlen(save_dir);
        size_t buffer_size = save_dir_length + 2;
        retro_save_dir=malloc(save_dir_length+2);
        strlcpy(retro_save_dir, save_dir, buffer_size);
        fill_pathname_slash(retro_save_dir, buffer_size);
    }

    directory_entries_map_lock = slock_new();
}

void cleanup_filesystem()
{
    free(content_basename);
    free(bgd_current_dir);
    free(retro_dir_root);
    free(retro_current_dir);
    free(retro_save_dir);

    for (size_t dir_idx = 0, dir_cap = RHMAP_CAP(directory_entries_map); dir_idx != dir_cap; dir_idx++)
    {
        if (RHMAP_KEY(directory_entries_map, dir_idx))
        {
            file_map_t* filemap = directory_entries_map[dir_idx].filemap;
            for (size_t file_idx = 0, file_cap = RHMAP_CAP(filemap); file_idx != file_cap; file_idx++)
            {
                if ( RHMAP_KEY(filemap, file_idx))
                {
                    free(filemap[file_idx].real_name);
                }
            }
            RHMAP_FREE(filemap);
        }
    }
    RHMAP_FREE(directory_entries_map);
    slock_free(directory_entries_map_lock);
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
    
    snprintf(buffer, PATH_MAX_LENGTH, "%s/%s", bgd_current_dir, dir);
    return buffer;
}

const char* to_retro_path(const char * dir)
{
    THREAD_LOCAL static char buffer[PATH_MAX_LENGTH];

    if (path_is_absolute(dir))
    {
        // Check if the passed directory is relative to the fake root directory
        if (strstr(dir, bgd_dir_root)==dir)
        {
            snprintf(buffer, PATH_MAX_LENGTH, "%s%s", retro_dir_root, dir+bgd_dir_root_len);
            return casepath(buffer, retro_dir_root_len+1);
        }
    }

    assert(false);
    return dir;
}

// bgd filesystem forwarded to libretro

RFILE * fopen_libretro ( const char * filename, const char * mode )
{
    const char* retro_filename = to_retro_path(resolve_bgd_path(filename));
    if (!retro_filename)
    {
        log_cb(RETRO_LOG_ERROR, "fopen_libretro: Could not find file %s\n", filename);
        return NULL;
    }

    return rfopen(retro_filename, mode);
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
    const char * retro_dir = to_retro_path(resolve_bgd_path(filename));
    return retro_dir ? filestream_delete(retro_dir) : -1;
}

int rename_libretro(const char *old_filename, const char *new_filename)
{
    const char * retro_old = to_retro_path(resolve_bgd_path(old_filename));
    const char * retro_new = to_retro_path(resolve_bgd_path(new_filename));

    return retro_old && retro_new ? filestream_rename( old_filename, new_filename) : -1;
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
    const char* bgd_dir = resolve_bgd_path(dir);
    const char * retro_dir = to_retro_path(bgd_dir);

    if (retro_dir)
    {
        if (path_is_directory(retro_dir))
        {
            free(retro_current_dir);
            retro_current_dir=strdup(retro_dir);
            free(bgd_current_dir);
            bgd_current_dir=strdup(bgd_dir);
            return 0;
        }
    }


    return -1;
}

int mkdir_libretro( const char * dir )
{
    const char * retro_dir = to_retro_path(resolve_bgd_path(dir));
    if (retro_dir)
    {
        // TODO: redirect to savedir
         return path_mkdir( retro_dir ) ? 0 : -1;
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

    const char * retro_dir = to_retro_path(buffer);

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

            const char * retro_filepath_full = to_retro_path(hdir->info.fullpath);
            hdir->info.size = path_get_size(retro_filepath_full);

            return true;
        }
    }

    return false;
}
