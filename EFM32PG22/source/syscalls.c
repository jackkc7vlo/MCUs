/**
 * @file syscalls.c
 * @brief Newlib syscall stubs for embedded systems
 *
 * These are minimal implementations of system calls required by newlib.
 * They are needed to avoid linker warnings and allow newlib functions to link.
 */

#include <errno.h>
#include <sys/stat.h>

#undef errno
extern int errno;

/**
 * @brief Close a file descriptor
 * @note Not implemented for embedded systems
 */
int _close(int file)
{
    (void)file;
    errno = ENOSYS;
    return -1;
}

/**
 * @brief Query whether a file descriptor is a terminal
 * @note Always returns false for embedded systems
 */
int _isatty(int file)
{
    (void)file;
    return 0;
}

/**
 * @brief Get status of a file
 * @note Not implemented for embedded systems
 */
int _fstat(int file, struct stat *st)
{
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}

/**
 * @brief Seek to a position in a file
 * @note Not implemented for embedded systems
 */
int _lseek(int file, int offset, int whence)
{
    (void)file;
    (void)offset;
    (void)whence;
    errno = ENOSYS;
    return -1;
}

/**
 * @brief Read from a file
 * @note Not implemented for embedded systems
 */
int _read(int file, char *ptr, int len)
{
    (void)file;
    (void)ptr;
    (void)len;
    errno = ENOSYS;
    return -1;
}

/**
 * @brief Write to a file
 * @note Implement this to redirect printf output (e.g., to UART)
 */
int _write(int file, char *ptr, int len)
{
    (void)file;
    (void)ptr;
    /* Implement UART output here if needed */
    return len;
}

/**
 * @brief Increase program data space
 * @note Basic heap implementation for malloc/free
 */
void *_sbrk(int incr)
{
    extern char  _end;         /* Defined by the linker script */
    extern char  __StackLimit; /* Defined by the linker script */
    static char *heap_end = 0;
    char        *prev_heap_end;

    if (heap_end == 0)
    {
        heap_end = &_end;
    }

    prev_heap_end = heap_end;

    if (heap_end + incr > &__StackLimit)
    {
        /* Heap overflow */
        errno = ENOMEM;
        return (void *)-1;
    }

    heap_end += incr;
    return (void *)prev_heap_end;
}

/**
 * @brief Exit the program
 * @note Enters infinite loop for embedded systems
 */
void _exit(int status)
{
    (void)status;
    while (1)
    {
        /* Infinite loop */
    }
}

/**
 * @brief Terminate a process
 * @note Not implemented for embedded systems
 */
int _kill(int pid, int sig)
{
    (void)pid;
    (void)sig;
    errno = ENOSYS;
    return -1;
}

/**
 * @brief Get process ID
 * @note Always returns 1 for embedded systems
 */
int _getpid(void)
{
    return 1;
}
