#include "memory.h"
#include "types.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void collect_memory_info(void)
{
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp)
        return;

    char line[256];
    unsigned long total = 0, available = 0;
    int found = 0;

    // Read both values in one pass
    while (fgets(line, sizeof(line), fp) && found < 2)
    {
        if (strncmp(line, "MemTotal:", 9) == 0)
        {
            sscanf(line, "MemTotal: %lu", &total);
            found++;
        }
        else if (strncmp(line, "MemAvailable:", 13) == 0)
        {
            sscanf(line, "MemAvailable: %lu", &available);
            found++;
        }
    }
    fclose(fp);

    if (total > 0)
    {
        g_system_info.memory.total_kb = total;
        g_system_info.memory.used_kb = total - available;
        g_system_info.memory.free_kb = available;
    }
}

void memory_info(void)
{
    float total_gb = g_system_info.memory.total_kb / (1024.0 * 1024.0);
    float used_gb = g_system_info.memory.used_kb / (1024.0 * 1024.0);

    char memory_usage[100];
    snprintf(memory_usage, sizeof(memory_usage), "%.1f GiB / %.1f GiB (%.1f%% used)",
             used_gb, total_gb, (used_gb / total_gb) * 100);

    format_row("Memory", memory_usage);
}