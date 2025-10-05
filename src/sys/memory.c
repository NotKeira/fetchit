/**
 * memory.c - Memory Information Collection
 *
 * Retrieves system memory statistics from /proc/meminfo, including
 * total, available, and used memory values. Calculates usage
 * percentages for display.
 */

#include "memory.h"
#include "types.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * collect_memory_info - Gather memory statistics
 *
 * Parses /proc/meminfo to extract total and available memory values.
 * Uses single-pass reading for efficiency, stopping after both values
 * are found. Calculates used memory from the difference between total
 * and available.
 *
 * Memory values are stored in kilobytes within the global system_info
 * structure.
 */
void collect_memory_info(void)
{
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp)
        return;

    char line[256];
    unsigned long total = 0, available = 0;
    int found = 0;

    /* Single-pass reading: stop after finding both values */
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

/**
 * memory_info - Display formatted memory information
 *
 * Converts memory values from kilobytes to gibibytes and displays
 * total, used, and percentage utilisation in a human-readable format.
 */
void memory_info(void)
{
    float total_gb = g_system_info.memory.total_kb / (1024.0 * 1024.0);
    float used_gb = g_system_info.memory.used_kb / (1024.0 * 1024.0);

    char memory_usage[100];
    snprintf(memory_usage, sizeof(memory_usage), "%.1f GiB / %.1f GiB (%.1f%% used)",
             used_gb, total_gb, (used_gb / total_gb) * 100);

    format_row("Memory", memory_usage);
}