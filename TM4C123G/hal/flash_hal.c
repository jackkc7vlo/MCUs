#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PAGE_SIZE 64u

typedef struct
{
    uint32_t size;
    uint8_t data[PAGE_SIZE];
} Page;

static volatile Page flash_page = {0u};

void flash_hal_write_page()
{
    if (flash_page.size > 0u)
    {
        FILE *file = fopen("flash.txt", "a");
        if (file == NULL)
        {
            perror("Failed to open file");
            exit(1);
        }
        fwrite((void const *)flash_page.data, sizeof(char), flash_page.size, file);
        fclose(file);
        flash_page.size = 0u;
        memset((void *)flash_page.data, 0xffu, PAGE_SIZE); // Initialize page->data with 0xff
    }
}

void flash_hal_shutdown()
{
    flash_hal_write_page();
}

void flash_hal_write(const uint8_t *data, uint32_t dataSize)
{
    uint32_t dataWritten = 0u;

    while (dataWritten < dataSize)
    {
        const uint32_t remainingSpace = PAGE_SIZE - flash_page.size;
        const uint32_t dataToWrite = (dataSize - dataWritten) < remainingSpace ? (dataSize - dataWritten) : remainingSpace;
        memcpy((void *)(flash_page.data + flash_page.size), data + dataWritten, dataToWrite);
        flash_page.size += dataToWrite;
        dataWritten += dataToWrite;

        if (flash_page.size == PAGE_SIZE)
        {
            flash_hal_write_page();
        }
    }
}

void flash_hal_initialize()
{
    memset((void *)flash_page.data, 0xff, PAGE_SIZE); // Initialize page->data with 0xff

    FILE *file = fopen("flash.txt", "r");
    if (file == NULL)
    {
        perror("Failed to open file");
        exit(1);
    }

    size_t bytesRead;
    while ((bytesRead = fread((void *)flash_page.data, sizeof(char), PAGE_SIZE, file)) == PAGE_SIZE)
    {
        // Do nothing, just read the file
    }

    // If the last data read is less than a page, it should place that data at the beginning of page.data
    if (bytesRead < PAGE_SIZE && bytesRead > 0u)
    {
        memmove((void *)flash_page.data, (void *)(flash_page.data + bytesRead), PAGE_SIZE - bytesRead);
        flash_page.size = bytesRead;
        // Fill the rest of the page with 0xff
        memset((void *)(flash_page.data + bytesRead), 0xff, PAGE_SIZE - bytesRead);
    }

    fclose(file);
}
