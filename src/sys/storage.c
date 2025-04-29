#include "storage.h"
#include "types.h"
#include "utils.h"
#include <stdio.h>
#include <sys/statvfs.h>

void storage_info()
{
    struct statvfs buf;
    if (statvfs("/", &buf) == 0)
    {
        // Calculate in one go to reduce floating point operations
        double total_gb = (double)(buf.f_blocks * buf.f_frsize) / (1024.0 * 1024.0 * 1024.0);
        double used_gb = (double)((buf.f_blocks - buf.f_bfree) * buf.f_frsize) / (1024.0 * 1024.0 * 1024.0);
        double usage_percent = (used_gb / total_gb) * 100;

        char storage_usage[100];
        snprintf(storage_usage, sizeof(storage_usage), "%.1f GiB / %.1f GiB (%.1f%% used)",
                 used_gb, total_gb, usage_percent);

        format_row("Storage", storage_usage);
    }
    else
    {
        format_row("Storage", "Error retrieving data");
    }
}

void collect_storage_info(void)
{
    struct statvfs buf;
    if (statvfs("/", &buf) == 0)
    {
        double total_gb = (double)(buf.f_blocks * buf.f_frsize) / (1024.0 * 1024.0 * 1024.0);
        double used_gb = (double)((buf.f_blocks - buf.f_bfree) * buf.f_frsize) / (1024.0 * 1024.0 * 1024.0);

        g_system_info.storage.total_gb = total_gb;
        g_system_info.storage.used_gb = used_gb;
        g_system_info.storage.free_gb = total_gb - used_gb;
    }
}