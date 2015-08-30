/** @file
    Mock functions to perform io operations used in tests.
    @author Piotr Rybicki
    @date 2015-08
  */

#include <string.h>
#include <stdio.h>
#include <setjmp.h>
#include <limits.h>
#include <cmocka.h>
#include <wchar.h>
#include <wctype.h>

#define IO_BUFFER_SIZE 16384 ///<Max number of bytes that can be stored in buffer.

static char io_buffer[IO_BUFFER_SIZE]; ///<Buffer used to store data saved by
static int io_save_position; ///<Indicator of the place in buffer where to save data.
static int io_load_position; ///<Indicator of the place in buffer where to load data from.

/**
 * @brief reset_io_buffer Fills buffer with 0s, resets io_*_position
 */
void reset_io_buffer(void)
{
    memset(io_buffer, WEOF, sizeof(char) * IO_BUFFER_SIZE);
    io_save_position = 0;
    io_load_position = 0;
}

/**
 * @brief get_io_buffer See return
 * @return Pointer to buffer array.
 */
char* get_io_buffer(void)
{
    return io_buffer;
}
/*
int get_io_position(void)
{
    return io_save_position;
}
*/
/**
 * @brief get_io_buffer_size Returns buffer max capacity.
 * @return Max capacity of the io_buffer.
 */
int get_io_buffer_size(void)
{
    return IO_BUFFER_SIZE;
}
/*
void reset_io_position(void)
{
    io_save_position = 0;
}
*/
/**
 * @brief testing_fputwc Mock version of fputwc
 * @param sign Sign to be put in buffer.
 * @param stream Fake stream to be compatible with real fputwc
 * @return Written wchar.
 */
wchar_t testing_fputwc (wchar_t sign, FILE * stream)
{
    assert_non_null(stream);
    assert_true(IO_BUFFER_SIZE >= io_save_position+sizeof(wchar_t)); //enough space
    memcpy(io_buffer+io_save_position, &sign, sizeof(wchar_t));
    io_save_position += sizeof(wchar_t);
    return sign;
}

/**
 * @brief testing_fgetwc Mock version of fgetwc
 * @param stream Fake stream to be compatible with real fgetwc.
 * @return Wchar read from buffer.
 */
wchar_t testing_fgetwc (FILE *stream)
{
    assert_non_null(stream);
    assert_true(IO_BUFFER_SIZE >= io_load_position+sizeof(wchar_t)); //no out-of-bounds reading
    wchar_t read;
    memcpy(&read, io_buffer+io_load_position, sizeof(wchar_t));
    io_load_position += sizeof(wchar_t);
    return read;
}
