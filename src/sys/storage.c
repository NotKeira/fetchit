#include "storage.h"
#include "utils.h"
#include <stdio.h>
#include <sys/statvfs.h>

void storage_info()
{
    struct statvfs buf;
    if (statvfs("/", &buf) == 0)
    {
        unsigned long block_size = buf.f_frsize;
        unsigned long total_size = buf.f_blocks * block_size;
        unsigned long free_size = buf.f_bfree * block_size;
        unsigned long used_size = total_size - free_size;
        float total_gb = total_size / (1024.0 * 1024.0 * 1024.0);
        float used_gb = used_size / (1024.0 * 1024.0 * 1024.0);

        char storage_usage[100];
        snprintf(storage_usage, sizeof(storage_usage), "%.1f GiB / %.1f GiB (%.1f%% used)", used_gb, total_gb, (used_gb / total_gb) * 100);

        format_row("Storage", storage_usage);
    }
    else
    {
        format_row("Storage", "Error retrieving data");
    }
}