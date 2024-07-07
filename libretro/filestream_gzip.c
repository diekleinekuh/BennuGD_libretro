#include "filestream_gzip.h"
#include "streams/trans_stream.h"
#include "streams/file_stream.h"
#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SKIP_STDIO_REDEFINES 1
#include "streams/file_stream_transforms.h"

extern const struct trans_stream_backend zlib_deflate_backend;
extern const struct trans_stream_backend zlib_inflate_backend;

#define BUFFER_SIZE 4096
#define WINDOW_BITS 31

struct gzFile_libretro
{
    void* stream;
    RFILE* filestream;
    const struct trans_stream_backend* backend;
    int64_t uncompressed_data_pos;
    unsigned int buffer_start_pos;
    unsigned int buffer_end_pos;    
    char buffer[BUFFER_SIZE];
};

gzFile_libretro* gzopen_libretro(const char* path, const char* mode)
{
    if (!path || !mode)
    {
        return NULL;
    }    

    bool writable = false;
    int level = 6; 
    if (strchr(mode, 'w'))
    {
        writable = true;
        for (const char* c=mode; *c!='\0'; ++c)
        {
            if ('0'<=*c && *c<='9')
            {
                level = *c - '0';
            }
        }
    }

    if (strchr(mode, '+') || (writable && strchr(mode, 'r')))
    {
        // opening for reading and writing not supported
        return NULL;
    }

    RFILE* filestream=rfopen(path, mode);
    if (!filestream)
    {
        return NULL;
    }

    gzFile_libretro* ret=(gzFile_libretro*)calloc(1, sizeof(gzFile_libretro));
    
    ret->filestream = filestream;
    ret->backend = writable ? &zlib_deflate_backend : &zlib_inflate_backend;
    ret->stream = ret->backend->stream_new();
    ret->backend->define(ret->stream, "window_bits", WINDOW_BITS);
    if (writable)
    {
        ret->backend->define(ret->stream, "level", level);
    }
    ret->buffer_start_pos = 0;
    ret->buffer_end_pos = 0 ;

    return ret;
}

int gzclose_libretro(gzFile_libretro* file)
{
    if (!file)
    {
        return Z_STREAM_ERROR;
    }

    // Flush the stream when writing
    int ret=Z_OK;
    if (file->backend == &zlib_deflate_backend)
    {
        
        while(true)
        {
            file->backend->set_in(file->stream, NULL, 0 ); // no more input data
            file->backend->set_out(file->stream, file->buffer+file->buffer_start_pos, BUFFER_SIZE - file->buffer_start_pos );

            uint32_t bytes_read, bytes_written;
            enum trans_stream_error error;
            bool success = file->backend->trans(file->stream, true, &bytes_read, &bytes_written, &error);
            file->buffer_start_pos += bytes_written;            
            if (!success)
            {
                ret=-1;
                break;
            }

            if (error == TRANS_STREAM_ERROR_NONE || error == TRANS_STREAM_ERROR_BUFFER_FULL)
            {
                if (file->buffer_start_pos != rfwrite(file->buffer, 1, file->buffer_start_pos, file->filestream))
                {
                    ret = -1;
                    break;
                }

                if (error == TRANS_STREAM_ERROR_NONE)
                {
                    break;
                }

                file->buffer_start_pos=0;                
            }
            else if (error == TRANS_STREAM_ERROR_AGAIN)
            {
                continue;
            }
            else
            {
                // Some other error
                ret = -1;
                break;
            }
        }
    }

    ret = rfclose(file->filestream);
    file->backend->stream_free(file->stream);
    free(file);

    return ret;
}

int64_t gzread_libretro(gzFile_libretro* file, void* buf, size_t len)
{
    if (!file)
    {
        return Z_STREAM_ERROR;
    }

    if (file->backend != &zlib_inflate_backend)
    {
        return Z_STREAM_ERROR;
    }

    if (file->stream==NULL)
    {
        return 0;
    }

    size_t bytes_read_overall=0;

    while (bytes_read_overall<len)
    {
        // Read new data if the input buffer is empty
        if (file->buffer_start_pos==file->buffer_end_pos)
        {
            file->buffer_end_pos = rfread(file->buffer, 1, BUFFER_SIZE, file->filestream);
            file->buffer_start_pos = 0;
        }

        // Read from buffer, write to buf
        file->backend->set_in(file->stream, file->buffer+file->buffer_start_pos, file->buffer_end_pos -file->buffer_start_pos );
        file->backend->set_out(file->stream, buf, len);        
        
        uint32_t bytes_read=0, bytes_written=0;
        enum trans_stream_error error;

        bool success = file->backend->trans(file->stream, false, &bytes_read, &bytes_written, &error);
        file->buffer_start_pos+=bytes_read;
        buf = (char*)buf + bytes_written;
        bytes_read_overall += bytes_written;
        file->uncompressed_data_pos+=bytes_written;

        if (!success)
        {
            break;
        }

        // Reached end of stream.
        if (error==TRANS_STREAM_ERROR_NONE)
        {
            file->backend->stream_free(file->stream);
            file->stream = NULL;
            break;
        }

        if (error!=TRANS_STREAM_ERROR_AGAIN)
        {
            break;
        }
    }

    return bytes_read_overall;
}

int gzeof_libretro(gzFile_libretro* file)
{
    if (!file)
    {
        return Z_STREAM_ERROR;
    }

    if (file->backend != &zlib_inflate_backend)
    {
        return Z_STREAM_ERROR;
    }

    return file->stream ? 0 : 1; // stream is being freed when reaching the end of the compressed data
}

int64_t gzwrite_libretro(gzFile_libretro* file, const void* buf, size_t len)
{
    if (!file)
    {
        return Z_STREAM_ERROR;
    }

    if (file->backend != &zlib_deflate_backend)
    {
        return Z_STREAM_ERROR;
    }

    size_t bytes_written_overall=0;

    while (bytes_written_overall<len)
    {
        // Read from buf, write to buffer
        file->backend->set_in(file->stream, buf, len);
        file->backend->set_out(file->stream, file->buffer+file->buffer_start_pos, BUFFER_SIZE - file->buffer_start_pos );

        uint32_t bytes_read=0, bytes_written=0;
        enum trans_stream_error error;

        bool success = file->backend->trans(file->stream, false, &bytes_read, &bytes_written, &error);
        
        buf = (char*)buf + bytes_read;        
        file->buffer_start_pos+=bytes_written;
        
        bytes_written_overall += bytes_read;
        file->uncompressed_data_pos+=bytes_read;

        if (!success)
        {
            break;
        }

        if (error==TRANS_STREAM_ERROR_BUFFER_FULL)
        {
            if (file->buffer_start_pos!=rfwrite(file->buffer, 1, file->buffer_start_pos, file->filestream))
            {
                // Could not write data, disk full?
                return -1;
            }

            file->buffer_start_pos = 0;
        }

        if (error!=TRANS_STREAM_ERROR_AGAIN)
        {
            break;
        }        
    }

    return bytes_written_overall;
}

long int gztell_libretro(gzFile_libretro* file )
{
    if (!file)
    {
        return -1;
    }

    return file->uncompressed_data_pos;
}

long int gzseek_libretro(gzFile_libretro* file, long int offset, int whence)
{
    if (!file)
    {
        return -1;
    }


    long long target_pos;

    switch(whence)
    {
        case SEEK_SET:
            target_pos = offset;
            break;
        case SEEK_CUR:
            target_pos = file->uncompressed_data_pos + offset;
        default:
            return -1;
    }

    if (target_pos<0)
    {
        return -1;
    }

    if (target_pos==file->uncompressed_data_pos)
    {
        return target_pos;
    }

    if (file->backend==&zlib_inflate_backend)
    {
        // Check if we seek backwards
        if (target_pos < file->uncompressed_data_pos || file->stream==NULL)
        {
            // Seek back the underlying file to the beginning
            if (rfseek(file->filestream, 0, SEEK_SET)!=0)
            {
                return -1;
            }

            // Recreate the compression stream.
            file->backend->stream_free(file->stream);
            file->stream=file->backend->stream_new();
            file->backend->define(file->stream, "window_bits", WINDOW_BITS);
            file->uncompressed_data_pos=0;
            file->buffer_end_pos = 0;
            file->buffer_start_pos = 0;
        }

        char buffer[4096];
        int64_t skip_bytes = target_pos - file->uncompressed_data_pos;
        while(skip_bytes>0)
        {            
            unsigned int read_chunk = sizeof(buffer);
            if (read_chunk>skip_bytes)
            {
                read_chunk=skip_bytes;
            }

            int64_t read_bytes = gzread_libretro(file, buffer, read_chunk);
            if (read_bytes!=read_chunk)
            {
                // Seeking past the end of the read only file, return the position. The next read will fail.
                break;
            }

            skip_bytes-=read_bytes;
        }
    }
    else
    {
         if (target_pos < file->uncompressed_data_pos)
         {
            return -1; // Seeking backwards not allowed in write mode
         }

        char buffer[4096]={0};
        int64_t skip_bytes = target_pos - file->uncompressed_data_pos;
        while(skip_bytes>0)
        {            
            unsigned int write_chunk = sizeof(buffer);
            if (write_chunk>skip_bytes)
            {
                write_chunk=skip_bytes;
            }

            int64_t written_bytes = gzwrite_libretro(file, buffer, write_chunk);
            if (written_bytes!=write_chunk)
            {
                // Seeking past the end of the read only file, return the position. The next read will fail.
                break;
            }

            skip_bytes-=written_bytes;
        }        
    }

    file->uncompressed_data_pos = target_pos;
    return target_pos;
}

int gzrewind_libretro(gzFile_libretro* file)
{
    return gzseek_libretro(file, 0, SEEK_SET);
}

char * gzgets_libretro(gzFile_libretro* file, char *buf, size_t len)
{
    if (file==NULL || buf==NULL || len<1 || file->backend!=&zlib_inflate_backend || file->stream==NULL)
    {
        return NULL;
    }    
    
    size_t max_chars = len -1;

    size_t i;
    for (i=0; i<max_chars; ++i)
    {
        if (gzread_libretro(file, buf+i, 1)!=1)
        {
            break;
        }

        if (buf[i]=='\n')
        {
            ++i;
            break;
        }
    }    

    if (i==0)
    {        
        return NULL;        
    }

    buf[i]='\0';
    return buf;
}

void test_gzip()
{
    char buffer[1024];
    gzFile_libretro* file = gzopen_libretro("/home/markus/test/test.gz", "r");

    int eof = gzeof_libretro(file);

    int read_bytes=gzread_libretro(file, buffer, 1024);

    eof = gzeof_libretro(file);

    int result = gzseek_libretro(file, 0, SEEK_SET);

    read_bytes=gzread_libretro(file, buffer, 1024);

    result = gzseek_libretro(file, 0, SEEK_SET);

    eof = gzeof_libretro(file);

    char *test = gzgets_libretro(file, buffer, 1024);

    eof = gzeof_libretro(file);

    gzclose_libretro(file);

    file = gzopen_libretro("/home/markus/test/test2.gz", "wb0");


    int bytes_written=gzwrite_libretro(file, "HalloArsch!",  11);
    gzclose_libretro(file);
    
    // FILE* raw_file = fopen("/home/markus/test/test.gz", "rb");
    // int raw_bytes=fread(buffer, 1, 1024, raw_file);
    // fclose(raw_file);    

    // const struct trans_stream_backend* backend = &zlib_inflate_backend;

    // void* stream = backend->stream_new();
    // bool worked = backend->define(stream, "window_bits", 16);

    // int input_ptr=0;
    // int output_ptr=0;

    // char out_buffer[1024];

    // enum trans_stream_error err;

    // do
    // {
    //     backend->set_in(stream, buffer+input_ptr, 1);
    //     backend->set_out(stream, out_buffer+output_ptr, 1024);

    //     uint32_t read_bytes=0, written_bytes=0;       

    //     bool trans_worked = backend->trans(stream, false, &read_bytes, &written_bytes, &err );
    //     input_ptr+=read_bytes;
    //     output_ptr+=written_bytes;
    // }
    // while(err==TRANS_STREAM_ERROR_AGAIN);

    // backend->stream_free(stream);


    // gzFile file = gzopen("/home/markus/test/test.gz", "rb");

    
    // int read = gzread(file, buffer, 1024);

    // gzclose(file);
}