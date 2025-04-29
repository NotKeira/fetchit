#include "memory.h"
#include "types.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned long get_meminfo_value(const char *field)
{
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp)
        return 0;

    char line[256];
    unsigned long value = 0;

    while (fgets(line, sizeof(line), fp))
    {
        if (strstr(line, field))
        {
            sscanf(line, "%*s %lu", &value);
            break;
        }
    }
    fclose(fp);
    return value;
}

void collect_memory_info(void)
{
    unsigned long total = get_meminfo_value("MemTotal:");
    unsigned long available = get_meminfo_value("MemAvailable:");

    if (total > 0)
    {
        unsigned long used = total - available;
        g_system_info.memory.total_kb = total;
        g_system_info.memory.used_kb = used;
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
