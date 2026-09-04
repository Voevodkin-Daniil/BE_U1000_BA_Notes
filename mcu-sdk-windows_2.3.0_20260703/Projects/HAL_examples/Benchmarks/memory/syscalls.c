/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/Benchmarks/memory/syscalls.c
 *  @author     Baikal electronics SDK team
 *  @brief      HAL example source file
 *  @version    2.3.0
 *  @date       20260703
 * *****************************************************************************
 *  @copyright Copyright (c) 2025-2026 Baikal Electronics
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <sys/stat.h>

extern int __io_putchar(int ch) __attribute__((weak));
extern int __io_getchar(void) __attribute__((weak));

/* Close a file. */
int _close(int file)
{
    (void) file;
    return -1;
}

/* Status of an open file. */
int _fstat(int file, struct stat *st)
{
    (void) file;
    st->st_mode = S_IFCHR;

    return 0;
}

/* Query whether output stream is a terminal. */
int _isatty(int file)
{
    (void) file;
    return 1;
}

/* Set position in a file. */
off_t _lseek(int file, off_t ptr, int dir)
{
    (void) file;
    (void) ptr;
    (void) dir;
    return 0;
}

/* Read from a file. */
ssize_t _read(int file, char *ptr, size_t len)
{
    (void) file;
    for (size_t i = 0; i < len; i++)
    {
        *ptr++ = __io_getchar();
    }

    return len;
}

/* Write to a file. */

ssize_t _write(int file, const char *ptr, size_t len)
{
    (void) file;
    for (size_t i = 0; i < len; i++)
    {
        __io_putchar(*ptr++);
    }

    return len;
}

extern char _heap_start[]; /* Start of heap */
char *heap_ptr;

/* Increase program data space. */
char* _sbrk(int nbytes)
{
    char *base;

    if (!heap_ptr)
        heap_ptr = (char*)&_heap_start;

    base = heap_ptr;
    heap_ptr += nbytes;

    return base;
}

/* Exit a program without cleaning up files. */
void _exit(int exit_status)
{
    (void) exit_status;
    while (1);
}

/* Send a signal. */
int _kill(int pid, int sig)
{
    (void) pid;
    (void) sig;
    errno = EINVAL;

    return -1;
}

/* Get process id. */
int _getpid()
{
    return 1;
}
