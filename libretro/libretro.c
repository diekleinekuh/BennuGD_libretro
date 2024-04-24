#include "libco.h"
#include "libco.h"
#include "libretro.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SDL.h>
#include "files_st.h"
#include <stdatomic.h>


static char* content_path;
static char* content_dir;
static size_t content_dir_length;
static retro_usec_t frametime_usec;
static bool use_audio_callback;
static void* audio_mixbuf;
static const size_t audio_frame_size=2*sizeof(int16_t);
static const size_t audio_mixbuf_frames=1024;

/**
 * find_last_slash:
 * @str                : path
 * @size               : size of path
 *
 * Find last slash in path. Tries to find
 * a backslash on Windows too which takes precedence
 * over regular slash.

 * @return pointer to last slash/backslash found in @str.
 **/
static char *find_last_slash(const char *str)
{
   const char *slash     = strrchr(str, '/');
#ifdef _WIN32
   const char *backslash = strrchr(str, '\\');

   if (!slash || (backslash > slash))
      return (char*)backslash;
#endif
   return (char*)slash;
}

static inline bool string_starts_with_size(const char *str, const char *prefix,
      size_t size)
{
   return (str && prefix) ? !strncmp(prefix, str, size) : false;
}


/**
 * path_is_absolute:
 * @path               : path
 *
 * Checks if @path is an absolute path or a relative path.
 *
 * @return true if path is absolute, false if path is relative.
 **/
static bool path_is_absolute(const char *path)
{
   if (!path || *path=='\0')
      return false;

   if (path[0] == '/')
      return true;

#if defined(_WIN32)
   /* Many roads lead to Rome...
    * Note: Drive letter can only be 1 character long */
   return ( string_starts_with_size(path,     "\\\\", STRLEN_CONST("\\\\"))
         || string_starts_with_size(path + 1, ":/",   STRLEN_CONST(":/")) 
         || string_starts_with_size(path + 1, ":\\",  STRLEN_CONST(":\\")));
#elif defined(__wiiu__) || defined(VITA)
   {
      const char *seperator = strchr(path, ':');
      return (seperator && (seperator[1] == '/'));
   }
#endif

   return false;
}

const char * libretro_adjustpath(const char* path)
{
    if (path_is_absolute(path))
    {
        return path;
    }

    size_t path_length = strlen(path);
    if (path_length + content_dir_length >= __MAX_PATH)
    {
        return path;
    }

    static char fixed_path[__MAX_PATH];
    strncpy(fixed_path, content_dir, __MAX_PATH);
    strncat(fixed_path, path, path_length);

    return fixed_path;
}

unsigned retro_api_version(void)
{
   return RETRO_API_VERSION;
}

static void default_log(enum retro_log_level level, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
}

void retro_get_system_info(struct retro_system_info *info)
{
   memset(info, 0, sizeof(*info));
   info->library_name     = "BennuGD";
   info->library_version  = "0.1";
   info->need_fullpath    = true;
   info->valid_extensions = "dat";
}

static retro_log_printf_t log_cb=&default_log;
static cothread_t main_thread;
static cothread_t bgd_thread;

static retro_video_refresh_t video_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_environment_t environ_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;

static char* retro_system_directory;
static char* retro_content_directory;
static char* retro_save_directory;


int libretro_width=320;
int libretro_height=200;
int libretro_depth=16;
char* libretro_base_dir;

static struct retro_system_av_info last_av_info;
static bool bgd_finished = false;
static bool game_unloading = false;
extern int fps_value;

void suspend_bgd()
{
    co_switch(main_thread);
}

void request_exit_bgd()
{
    if (!game_unloading)
    {
        environ_cb(RETRO_ENVIRONMENT_SHUTDOWN, NULL);
        co_switch(main_thread);
    }
}

short int libretro_input_state_cb(unsigned port,unsigned device,unsigned index,unsigned id)
{
    if (input_state_cb)
    {
	    return input_state_cb(port,device,index,id);
    }

    return 0;
}

extern int sdl_libretro_get_sample_rate();
extern void sdl_libretro_init_audio();
extern void sdl_libretro_cleanup_audio();
extern void sdl_libretro_runaudio(void* mixbuf, size_t mixbuf_size);
static void RETRO_CALLCONV retro_audio_callback(void)
{   
    sdl_libretro_runaudio(audio_mixbuf, audio_mixbuf_frames*audio_frame_size);
    audio_batch_cb((int16_t*)audio_mixbuf, audio_mixbuf_frames);
}

void RETRO_CALLCONV retro_frame_time_callback(retro_usec_t usec)
{
    frametime_usec = usec;
}

static void RETRO_CALLCONV retro_upload_audio()
{
    uint64_t sample_rate = last_av_info.timing.sample_rate;
    size_t frames = (frametime_usec * sample_rate)/ 1000000;

    while(frames)
    {
        size_t chunk = frames;
        if (chunk>audio_mixbuf_frames)
        {
            chunk = audio_mixbuf_frames;
        }

        sdl_libretro_runaudio(audio_mixbuf, chunk*audio_frame_size);
        audio_batch_cb((int16_t*)audio_mixbuf, chunk);
        frames -= chunk;
    }
}

void RETRO_CALLCONV retro_audio_set_state_callback(bool enabled)
{
    if (use_audio_callback==enabled)
    {
        return;
    }

    use_audio_callback = enabled;
}

void retro_set_video_refresh(retro_video_refresh_t cb) { video_cb = cb; }
void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) { audio_batch_cb = cb; }
void retro_set_audio_sample(retro_audio_sample_t cb) {  }
void retro_set_input_poll(retro_input_poll_t cb) { input_poll_cb = cb; }
void retro_set_input_state(retro_input_state_t cb) { input_state_cb = cb; }

void retro_set_environment(retro_environment_t cb)
{
    environ_cb = cb;
    
    struct retro_log_callback logging;

    if (cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &logging))
    {
        log_cb = logging.log;
    }
    else
    {
        log_cb = default_log;
    }   
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
    *info = last_av_info;
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
   log_cb(RETRO_LOG_INFO, "Plugging device %u into port %u.\n", device, port);
}

void retro_reset(void)
{

}

size_t retro_serialize_size(void)
{
   return 0;
}

bool retro_serialize(void *data_, size_t size)
{
   return false;
}

bool retro_unserialize(const void *data_, size_t size)
{
   return false;
}

void *retro_get_memory_data(unsigned id)
{
   return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
   return 0;
}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
}

void retro_cheat_reset(void)
{

}

extern int bgdi_main(int argc, char*argv[]);
static void run_bennugd()
{
    char *argv[2]={ "bgdi", content_path};
    bgdi_main(sizeof(argv)/sizeof(*argv), argv);

    bgd_finished = true;

    while(true)
    {
        co_switch(main_thread);
    }
}

void retro_init(void)
{
    last_av_info.geometry.base_width   = libretro_width;
    last_av_info.geometry.base_height  = libretro_height;
    last_av_info.geometry.max_width    = libretro_width;
    last_av_info.geometry.max_height   = libretro_height;
    last_av_info.geometry.aspect_ratio = 0.0f;
    last_av_info.timing.sample_rate = 44100;
    last_av_info.timing.fps = 25; // default value in bgd

    const char *system_dir = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &system_dir) && system_dir)
    {
        retro_system_directory=strdup(system_dir);		
    }		   

    const char *content_dir = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_CONTENT_DIRECTORY, &content_dir) && content_dir)
    {			
          retro_content_directory=strdup(content_dir);
    }
    
    const char *save_dir = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY, &save_dir) && save_dir)
    {
        retro_save_directory = strdup(*save_dir ? save_dir : retro_system_directory);
    }
    else
    {
        retro_save_directory=strdup(retro_system_directory);
    }

    enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
    //if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
    {
        log_cb(RETRO_LOG_WARN, "RETRO_PIXEL_FORMAT_XRGB8888 not supported.\n");
        fmt = RETRO_PIXEL_FORMAT_RGB565;
        if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
        {
            log_cb(RETRO_LOG_WARN, "RETRO_PIXEL_FORMAT_RGB565 not supported.\n");
            libretro_depth=15;
        }
        else
        {
            libretro_depth=16;
        }
    }

    // struct retro_input_descriptor input_descriptors[] = 
    // {
    //     { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A, "A" },
    //     { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B, "B" },
    //     { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X, "X" },
    //     { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y, "Y" },
    //     { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },
    //     { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },
    //     { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right" },
    //     { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT, "Left" },
    //     { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP, "Up" },
    //     { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN, "Down" },
    //     { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R, "R" },
    //     { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L, "L" },
    //     { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2, "R2" },
    //     { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2, "L2" },
    //     { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R3, "R3" },
    //     { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L3, "L3" }
    // };
    // environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, &input_descriptors);
    
    audio_mixbuf = malloc(audio_mixbuf_frames*audio_frame_size);
    sdl_libretro_init_audio();

    environ_cb(RETRO_ENVIRONMENT_SET_AUDIO_CALLBACK, &(struct retro_audio_callback){ 
        &retro_audio_callback, 
        &retro_audio_set_state_callback
        } );

    if (!main_thread)
    {
        main_thread = co_active();
        bgd_thread  = co_create(16*65536*sizeof(void*), &run_bennugd);
    }
}

bool retro_load_game(const struct retro_game_info *info)
{
    if (!info->path)
    {
        return false;
    }

    content_path=strdup(info->path);

    char * last_slash = find_last_slash(content_path);
    if (last_slash)
    {
        content_dir_length = last_slash - content_path + 1;
        content_dir=strndup( content_path, content_dir_length);
    }

    environ_cb(RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK, &(struct retro_frame_time_callback) 
    {
        &retro_frame_time_callback
    });

    co_switch(bgd_thread);    
    return true;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num)
{
    return false;
}

void retro_unload_game(void)
{
    // This notifies bgd that the main loop should quit
    extern int must_exit;
    must_exit = 1;
    game_unloading = true;
    while (bgd_finished==false)
    {
        co_switch(bgd_thread);
    }
    game_unloading = false;

    if (content_path)
    {
        free(content_path);
        content_path = NULL;
    }

    if (content_dir)
    {
        free(content_dir);
        content_dir = NULL;
    }
}

unsigned retro_get_region(void)
{
   return RETRO_REGION_NTSC;
}

void retro_run(void)
{
    co_switch(bgd_thread);
    SDL_Surface* s = SDL_GetVideoSurface();
    if (video_cb && s)
    {
        int sample_rate = sdl_libretro_get_sample_rate();

        if (s->w != last_av_info.geometry.base_width ||  s->h != last_av_info.geometry.base_height || fps_value!=last_av_info.timing.fps || 
            last_av_info.timing.sample_rate != sample_rate)
        {
            last_av_info.geometry.base_width = s->w;
            last_av_info.geometry.base_height = s->h;            

            if (s->w > last_av_info.geometry.max_width || s->h > last_av_info.geometry.max_height || fps_value!=last_av_info.timing.fps ||
                last_av_info.timing.sample_rate != sample_rate)
            {
                last_av_info.geometry.max_width = s->w;
                last_av_info.geometry.max_height = s->h;
                last_av_info.timing.fps = fps_value;
                last_av_info.timing.sample_rate = sample_rate;
                environ_cb(RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO, &last_av_info);
            }
            else
            {
                 environ_cb(RETRO_ENVIRONMENT_SET_GEOMETRY, &last_av_info.geometry);
            }  
        }

        video_cb(s->pixels, s->w, s->h, s->pitch);
    }

    if (!use_audio_callback)
    {
        retro_upload_audio();
    }
}

void retro_deinit(void)
{
    sdl_libretro_cleanup_audio();
    free(audio_mixbuf);
    free(retro_system_directory);
    free(retro_content_directory);
    free(retro_save_directory);
    co_delete(bgd_thread);
}
