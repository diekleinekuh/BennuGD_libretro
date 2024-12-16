#include "libco.h"
#include "libco.h"
#include "libretro.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SDL.h>
#include "filesystem.h"
#include "compat/posix_string.h"
#include <assert.h>

static struct retro_vfs_interface_info retro_vfs_interface_info = { 3, NULL};

static retro_usec_t frametime_usec;
static bool use_audio_callback;
static void* audio_mixbuf;
static const size_t audio_frame_size=2*sizeof(int16_t);
static const size_t audio_mixbuf_frames=1024;


static struct retro_perf_callback retro_perf_interface;

uint64_t retro_get_microseconds()
{
    if (retro_perf_interface.get_time_usec)
    {
        return retro_perf_interface.get_time_usec();
    }

    return 0;
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
   info->valid_extensions = "dat|exe";
}

retro_log_printf_t log_cb=&default_log;
static cothread_t main_thread;
static cothread_t bgd_thread;

static retro_video_refresh_t video_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_environment_t environ_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;


int libretro_width=320;
int libretro_height=200;
int libretro_depth=16;
char* libretro_base_dir;
bool retro_enable_frame_limiter=true;
bool force_frame_limiter=false;

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

// Mouse emulation
enum mouse_emulation_mode
{
    MOUSE_EMULATION_OFF=-1,
    MOUSE_EMULATION_LEFT_ANALOG=RETRO_DEVICE_INDEX_ANALOG_LEFT,
    MOUSE_EMULATION_RIGHT_ANALOG=RETRO_DEVICE_INDEX_ANALOG_RIGHT
};

static enum mouse_emulation_mode mouse_emulation = MOUSE_EMULATION_OFF;

static unsigned mouse_emulation_port = 0;
static float deadzone = 0.1f;
static float scale = 10.0;


typedef struct mouse_button_mapping
{
    unsigned mouse_button_id;
    const char* variable;
    const char* label;
    const char* desc;
    int default_value;
} mouse_button_mapping_t;

#define BGD_CORE_OPTION(a) "bennugd_"a

const char * force_frame_limiter_opt = BGD_CORE_OPTION("force_frame_limiter");
const char * mouse_emulation_opt = BGD_CORE_OPTION("mouse_emulation");
const char * mouse_emulation_off_optval = "off";
const char * mouse_emulation_left_analog_optval = "left analog";
const char * mouse_emulation_right_analog_optval = "right analog";

const char * mouse_emulation_dead_zone_opt = BGD_CORE_OPTION("mouse_emulation_dead_zone");
const char * mouse_emulation_scaling_opt = BGD_CORE_OPTION("mouse_emulation_scale");



static mouse_button_mapping_t mouse_button_mappings[]=
{
    { RETRO_DEVICE_ID_MOUSE_LEFT,       BGD_CORE_OPTION("mouse_emulation_left"),     "Mouse left",       "Left mouse button",       RETRO_DEVICE_ID_JOYPAD_B},
    { RETRO_DEVICE_ID_MOUSE_RIGHT,      BGD_CORE_OPTION("mouse_emulation_right"),    "Mouse right",      "Right mouse button",      RETRO_DEVICE_ID_JOYPAD_A},
    { RETRO_DEVICE_ID_MOUSE_MIDDLE,     BGD_CORE_OPTION("mouse_emulation_middle"),   "Mouse middle",     "Middle mouse button",                           -1},
    { RETRO_DEVICE_ID_MOUSE_BUTTON_4,   BGD_CORE_OPTION("mouse_emulation_button4"),  "Mouse button 4",   "Forth mouse button",                            -1},
    { RETRO_DEVICE_ID_MOUSE_BUTTON_5,   BGD_CORE_OPTION("mouse_emulation_button5"),  "Mouse button 5",   "Fifth mouse button",                            -1},
    { RETRO_DEVICE_ID_MOUSE_WHEELUP,    BGD_CORE_OPTION("mouse_emulation_wheelup"),  "Mouse wheel up",   "Mouse wheel scroll up",                         -1},
    { RETRO_DEVICE_ID_MOUSE_WHEELDOWN,  BGD_CORE_OPTION("mouse_emulation_wheeldown"),"Mouse wheel down", "Mouse wheel scroll down",                       -1},
};

#undef BGD_CORE_OPTION

typedef struct input_mapping
{
    const char * option_value;
    unsigned id;
    bool inverted; // makes only sense for an axis
} input_mapping_t;

input_mapping_t button_inputs[]=
{
    { "---",    -1                              },
    { "A",      RETRO_DEVICE_ID_JOYPAD_A        },
    { "B",      RETRO_DEVICE_ID_JOYPAD_B        },
    { "X",      RETRO_DEVICE_ID_JOYPAD_X        },
    { "Y",      RETRO_DEVICE_ID_JOYPAD_Y        },
    { "L",      RETRO_DEVICE_ID_JOYPAD_L        },
    { "R",      RETRO_DEVICE_ID_JOYPAD_R        },
    { "L2",     RETRO_DEVICE_ID_JOYPAD_L2       },
    { "R2",     RETRO_DEVICE_ID_JOYPAD_R2       },
    { "L3",     RETRO_DEVICE_ID_JOYPAD_L3       },
    { "R3",     RETRO_DEVICE_ID_JOYPAD_R3       },
    { "Select", RETRO_DEVICE_ID_JOYPAD_SELECT   },
    { "Start",  RETRO_DEVICE_ID_JOYPAD_START    },
    { "Up",     RETRO_DEVICE_ID_JOYPAD_UP       },
    { "Down",   RETRO_DEVICE_ID_JOYPAD_DOWN     },
    { "Left",   RETRO_DEVICE_ID_JOYPAD_LEFT     },
    { "Right",  RETRO_DEVICE_ID_JOYPAD_RIGHT    }
};

struct retro_core_option_definition get_mouse_button_mapping(unsigned index)
{
    assert(index<sizeof(mouse_button_mappings)/sizeof(mouse_button_mappings[0]));

    const unsigned num_button_inputs = sizeof(button_inputs)/sizeof(button_inputs[0]);

    const char* default_value = NULL;
    for (unsigned i=0; i<num_button_inputs; ++i)
    {
        if (button_inputs[i].id == mouse_button_mappings[index].default_value)
        {
            default_value = button_inputs[i].option_value;
            break;
        }
    }

    struct retro_core_option_definition result =
    {
        .default_value = default_value,
        .key = mouse_button_mappings[index].variable,
        .info = mouse_button_mappings[index].desc,
        .desc = mouse_button_mappings[index].label
    };

    for (unsigned i=0; i<num_button_inputs  ; ++i )
    {
        result.values[i].value = button_inputs[i].option_value;
        result.values[i].label = NULL;
    }

    result.values[num_button_inputs].label = NULL;
    result.values[num_button_inputs].value = NULL;

    return result;
}


int mouse_emulation_mappings[RETRO_DEVICE_ID_MOUSE_BUTTON_5+1];

static size_t write_legacy_option(char* buffer, size_t buffer_len, const struct retro_core_option_definition* option)
{
    assert(option);
    assert(option->values[0].value);

    unsigned current_option = 0;
    size_t written = snprintf(buffer, buffer_len, "%s; %s", option->desc, option->values[current_option++].value);
    size_t written_total = written;

    while(option->values[current_option].value)
    {
        if (buffer && buffer_len)
        {
            buffer+=written;
            buffer_len-=written;
        }

        written = snprintf(buffer, buffer_len, "|%s", option->values[current_option].value);
        written_total += written;
        ++current_option;
    }

    return written_total;
}

static void set_core_options()
{
    #define DECILE(b,p) { b"0", b"0"p}, { b"1", b"1"p}, { b"2", b"2"p}, { b"3", b"3"p}, { b"4", b"4"p}, { b"5", b"5"p}, { b"6", b"6"p}, { b"7", b"7"p}, { b"8", b"8"p}, { b"9", b"9"p}
    #define D_PERCENT(b) DECILE(b,"%")
    #define D_PX(b) DECILE(b,"px")


    struct retro_core_option_definition options[] =
    {
        {
            .key =  force_frame_limiter_opt,
            .desc= "Frame limiter",
            .info = "Force internal frame limiter even if content and frontend frame rate matches",
            .default_value = "false",
            .values = { { "true", "True"}, { "false", "False"}, { NULL, NULL} }
        },
        {
            .key = mouse_emulation_opt,
            .desc = "Mouse emulation",
            .info = "Mouse emulation mode",
            .default_value = mouse_emulation_off_optval,
            .values = { 
                { mouse_emulation_off_optval, mouse_emulation_off_optval}, 
                { mouse_emulation_left_analog_optval, mouse_emulation_left_analog_optval}, 
                { mouse_emulation_right_analog_optval, mouse_emulation_right_analog_optval}, 
                { NULL, NULL} }
        },
        {
            .key = mouse_emulation_dead_zone_opt,
            .desc = "Mouse emulation dead zone",
            .info = "Dead zone in percent",
            .default_value = "10",
            .values = { D_PERCENT(""), D_PERCENT("1"), D_PERCENT("2"), D_PERCENT("3"), D_PERCENT("4"), D_PERCENT("5"), D_PERCENT("6"), D_PERCENT("7"), D_PERCENT("8"), D_PERCENT("9"), { "100", "100%"},{ NULL, NULL} }
        },
        {
            .key = mouse_emulation_scaling_opt,
            .desc = "Mouse emulation scaling",
            .info = "Scaling in pixels",
            .default_value = "20",
            .values = { D_PX(""), D_PX("1"), D_PX("2"), D_PX("3"), D_PX("4"), D_PX("5"), D_PX("6"), D_PX("7"), D_PX("8"), D_PX("9"), { "100", "100px"},{ NULL, NULL} }
        },        
        get_mouse_button_mapping(0),
        get_mouse_button_mapping(1),
        get_mouse_button_mapping(2),
        get_mouse_button_mapping(3),
        get_mouse_button_mapping(4),
        get_mouse_button_mapping(5),
        get_mouse_button_mapping(6),
        {NULL}
    };
    #undef DECILE

    // check options version
    unsigned core_options_version=0;
    environ_cb(RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION, &core_options_version);

    if (core_options_version)
    {
        if (!environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS, options))
        {
            log_cb(RETRO_LOG_WARN, "RETRO_ENVIRONMENT_SET_CORE_OPTIONS failed");
        }
    }
    else
    {
        struct retro_variable variables[sizeof(options)/sizeof(options[0])] = {};

        for ( unsigned i = 0; i<sizeof(options)/sizeof(options[0])-1; ++i)
        {
            const struct retro_core_option_definition* option = options + i;

            variables[i].key = option->key;

            size_t buffer_size = write_legacy_option(NULL, 0, option);
            char* buffer = malloc(buffer_size + 1);            
            write_legacy_option(buffer, buffer_size, option);
            variables[i].value = buffer;
        }
        
        if (!environ_cb(RETRO_ENVIRONMENT_SET_VARIABLES, variables))
        {
            log_cb(RETRO_LOG_WARN, "RETRO_ENVIRONMENT_SET_VARIABLES failed");
        }

        for ( unsigned i = 0; i<sizeof(options)/sizeof(options[0]); ++i)
        {
            free((void*)variables[i].value);
        }
    }
}

input_mapping_t axis_inputs[]=
{
    { "Analog X",           RETRO_DEVICE_ID_ANALOG_X },
    { "Analog Y",           RETRO_DEVICE_ID_ANALOG_Y },
    { "Analog Y Inverted",  RETRO_DEVICE_ID_ANALOG_Y, true }
};

int analog_to_mouse(int16_t input)
{
    float normalized = (float)input / (float)0x8000;

    if (fabsf(normalized)<deadzone)
    {
        return 0;
    }

    return normalized*scale;
}

short int libretro_input_state_cb(unsigned port,unsigned device,unsigned index,unsigned id)
{
    if (input_state_cb)
    {
        if (mouse_emulation!=MOUSE_EMULATION_OFF && 0==port && device==RETRO_DEVICE_MOUSE && index==0)
        {
            if (id<sizeof(mouse_emulation_mappings)/sizeof(mouse_emulation_mappings[0]))
            {
                switch(id)
                {
                    case RETRO_DEVICE_ID_MOUSE_X:
                        return analog_to_mouse(input_state_cb(mouse_emulation_port, RETRO_DEVICE_ANALOG, mouse_emulation, RETRO_DEVICE_ID_ANALOG_X));
                    case RETRO_DEVICE_ID_MOUSE_Y:
                        return analog_to_mouse(input_state_cb(mouse_emulation_port, RETRO_DEVICE_ANALOG, mouse_emulation, RETRO_DEVICE_ID_ANALOG_Y));
                    default:
                        {
                            const int remapped_id = mouse_emulation_mappings[id];
                            if (remapped_id>=0)
                            {
                                return input_state_cb(mouse_emulation_port, RETRO_DEVICE_JOYPAD, 0, remapped_id);
                            }
                        }
                }
            }
        }

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


static const char* get_option_value(const char* option_name)
{
    struct retro_variable retro_variable = { option_name, NULL };
    if ( environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &retro_variable) )
    {
        if ( retro_variable.value )
        {
            return retro_variable.value;
        }
    }

    return NULL;
}

static bool get_boolean_option(const char* option_name, bool default_value)
{
    const char * value = get_option_value(option_name);
    if (value)
    {
        if (stricmp(value, "true")==0)
        {
            return true;
        }

        if (stricmp(value, "false")==0)
        {
            return false;
        }
    }

    return default_value;
}

static void update_mouse_button_option(mouse_button_mapping_t* mapping)
{
    assert(mapping);
    assert(mapping->variable);

    const char* value = get_option_value(mapping->variable);
    if (value)
    {
        for (int i=0; i<sizeof(button_inputs)/sizeof(button_inputs[0]); ++i)
        {
            if (0==stricmp(value, button_inputs[i].option_value))
            {
                mouse_emulation_mappings[mapping->mouse_button_id] = button_inputs[i].id;
                return;
            }
        }
    }

    mouse_emulation_mappings[mapping->mouse_button_id] = mapping->default_value; 
}

static void update_variables()
{
    // Frame limiter
    force_frame_limiter = get_boolean_option(force_frame_limiter_opt, false);

    // Mouse emulation mode
    const char* mouse_emulation_option=get_option_value(mouse_emulation_opt);
    if (mouse_emulation_option)
    {
        if (0==stricmp(mouse_emulation_option, mouse_emulation_off_optval))
        {
            mouse_emulation = MOUSE_EMULATION_OFF;
        }
        else if (0==stricmp(mouse_emulation_option, mouse_emulation_left_analog_optval))
        {
            mouse_emulation = MOUSE_EMULATION_LEFT_ANALOG;
        }
        else if (0==stricmp(mouse_emulation_option, mouse_emulation_right_analog_optval))
        {
            mouse_emulation = MOUSE_EMULATION_RIGHT_ANALOG;
        }
    }
    else
    {
         mouse_emulation = MOUSE_EMULATION_OFF;
    }

    // Mouse emulation analog stick dead zone
    {
        const char* mouse_emulation_dead_zone_option=get_option_value(mouse_emulation_dead_zone_opt);
        if (mouse_emulation_dead_zone_option)
        {
            int deadzone_int=0;
            if (1==sscanf(mouse_emulation_dead_zone_option, "%d", &deadzone_int))
            {
                deadzone = ((float)deadzone_int)/100.0f;
            }
        }
    }

    // Mouse emulation scaling
    {
        const char* mouse_emulation_scaling_option=get_option_value(mouse_emulation_scaling_opt);
        if (mouse_emulation_scaling_option)
        {
            int scaling=0;
            if (1==sscanf(mouse_emulation_scaling_option, "%d", &scaling))
            {
                scale = (float)scaling;
            }
        }
    }

    for (unsigned i = 0; i < sizeof(mouse_button_mappings)/sizeof(mouse_button_mappings[0]); ++i)
    {        
        update_mouse_button_option(&mouse_button_mappings[i]);
    }
}

void retro_set_environment(retro_environment_t cb)
{
    environ_cb = cb;
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
static void run_bennugd(void)
{
    char* arg0=get_content_basename();
    char* arg1=NULL;
    int argc = 1;
    char * extension=strstr(arg0, ".");

    if (extension)
    {
        if (strcasecmp(extension, ".exe"))
        {
            arg1 = arg0;
            arg0="bgdi";
            argc=2;
        }
    }

    char *argv[2]={ arg0, arg1};
    bgdi_main(argc, argv);

    bgd_finished = true;

    while(true)
    {
        co_switch(main_thread);
    }
}

void retro_init(void)
{
    struct retro_log_callback logging;

    if (environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &logging))
    {
        log_cb = logging.log;
    }
    else
    {
        log_cb = default_log;
    }

    set_core_options();

    update_variables();

    last_av_info.geometry.base_width   = libretro_width;
    last_av_info.geometry.base_height  = libretro_height;
    last_av_info.geometry.max_width    = libretro_width;
    last_av_info.geometry.max_height   = libretro_height;
    last_av_info.geometry.aspect_ratio = 0.0f;
    last_av_info.timing.sample_rate = 44100;
    last_av_info.timing.fps = 60;
    
    float frontend_refresh_rate = 0;
    if (environ_cb(RETRO_ENVIRONMENT_GET_TARGET_REFRESH_RATE, &frontend_refresh_rate))
    {
        last_av_info.timing.fps = frontend_refresh_rate;
    }

    if (environ_cb(RETRO_ENVIRONMENT_GET_VFS_INTERFACE, &retro_vfs_interface_info))
    {
        log_cb(RETRO_LOG_INFO, "Using vfs interface");
    }

    if (environ_cb(RETRO_ENVIRONMENT_GET_PERF_INTERFACE, &retro_perf_interface))
    {
        log_cb(RETRO_LOG_INFO, "retrieved perf interface");
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


    static const struct retro_controller_description controller_descriptions[] =
    {
        { "Retropad", RETRO_DEVICE_JOYPAD },
        { "Keyboard", RETRO_DEVICE_KEYBOARD },
        { "Mouse", RETRO_DEVICE_MOUSE },
        { "Analog", RETRO_DEVICE_ANALOG },
        { "None", RETRO_DEVICE_NONE }
    };

#define NUM_CONTROLLER_DESCRIPTION_ENTRIES (sizeof(controller_descriptions)/sizeof(controller_descriptions[0]))
    static struct retro_controller_info controller_info[] =
    {
        { controller_descriptions, NUM_CONTROLLER_DESCRIPTION_ENTRIES },
        { controller_descriptions, NUM_CONTROLLER_DESCRIPTION_ENTRIES },
        { controller_descriptions, NUM_CONTROLLER_DESCRIPTION_ENTRIES },
        { controller_descriptions, NUM_CONTROLLER_DESCRIPTION_ENTRIES },
        { NULL, 0 }
    };
#undef NUM_CONTROLLER_DESCRIPTION_ENTRIES

    if (!environ_cb(RETRO_ENVIRONMENT_SET_CONTROLLER_INFO, controller_info))
    {
        log_cb(RETRO_LOG_ERROR, "RETRO_ENVIRONMENT_SET_CONTROLLER_INFO failed");
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

    if (!environ_cb(RETRO_ENVIRONMENT_SET_AUDIO_CALLBACK, &(struct retro_audio_callback){
        &retro_audio_callback,
        &retro_audio_set_state_callback
        } ))
    {
        log_cb(RETRO_LOG_WARN, "RETRO_ENVIRONMENT_SET_AUDIO_CALLBACK failed");
    }

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

    const char *save_dir = NULL;

    if (!environ_cb(RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY, &save_dir) ||  !save_dir)
    {
        environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &save_dir);
    }

    init_filesystem( info->path, save_dir, &retro_vfs_interface_info );

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
}

unsigned retro_get_region(void)
{
   return RETRO_REGION_NTSC;
}

void retro_run(void)
{
    bool update_varaibles = false;
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &update_varaibles))
    {
        if (update_varaibles)
        {
            update_variables();
        }
    }

    co_switch(bgd_thread);
    SDL_Surface* s = SDL_GetVideoSurface();
    if (video_cb && s)
    {
        int sample_rate = sdl_libretro_get_sample_rate();

        if (s->w != last_av_info.geometry.base_width ||  s->h != last_av_info.geometry.base_height || fps_value>last_av_info.timing.fps ||
            last_av_info.timing.sample_rate != sample_rate)
        {
            last_av_info.geometry.base_width = s->w;
            last_av_info.geometry.base_height = s->h;

            bool submit_new_avinfo = false;
            if (s->w > last_av_info.geometry.max_width)
            {
                last_av_info.geometry.max_width = s->w;
                submit_new_avinfo = true;
            }

            if (s->h > last_av_info.geometry.max_height)
            {
                last_av_info.geometry.max_height = s->h;
                submit_new_avinfo = true;
            }

            if (fps_value>last_av_info.timing.fps)
            {
                last_av_info.timing.fps = fps_value;
                submit_new_avinfo = true;
            }

            if (last_av_info.timing.sample_rate != sample_rate)
            {
                last_av_info.timing.sample_rate = sample_rate;
                submit_new_avinfo = true;
            }

            if (submit_new_avinfo)
            {
                environ_cb(RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO, &last_av_info);
            }
            else
            {
                environ_cb(RETRO_ENVIRONMENT_SET_GEOMETRY, &last_av_info.geometry);
            }
        }

        if (fps_value != last_av_info.timing.fps || force_frame_limiter)
        {
            retro_enable_frame_limiter = true;
        }
        else
        {
            retro_enable_frame_limiter = false;
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
    co_delete(bgd_thread);
    cleanup_filesystem();
}
