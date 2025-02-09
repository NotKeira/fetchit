#include "memory.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// static void parse_meminfo(char *line, const char *key, unsigned long long *value) {
//     char *match;
//     if ((match = strstr(line, key)) != NULL) {
//         *value = strtoull(match + strlen(key), NULL, 10);
//     }
// }

// static int read_memory_stats(unsigned long long *total, unsigned long long *free_mem, unsigned long long *avail) {
//     FILE *fp = fopen("/proc/meminfo", "r");
//     if (!fp) return 0;

//     char line[256];
//     while (fgets(line, sizeof(line), fp)) {
//         parse_meminfo(line, "MemTotal:", total);
//         parse_meminfo(line, "MemFree:", free_mem);
//         parse_meminfo(line, "MemAvailable:", avail);
//     }
//     fclose(fp);
//     return 1;
// }
void memory_info()
{
    char *output;
    float total, used, free_memory;

    output = get_command_output("free -g | grep Mem | awk '{print $2, $3, $4}'");
    if (output != NULL)
    {
        sscanf(output, "%f %f %f", &total, &used, &free_memory);
        free(output);

        char memory_usage[100];
        snprintf(memory_usage, sizeof(memory_usage), "%.1f GiB / %.1f GiB (%.1f%% used)", used, total, (used / total) * 100);

        format_row("Memory", memory_usage);
        return;
    }

    output = get_command_output("sysctl -n hw.memsize");
    if (output != NULL)
    {
        unsigned long long total_mem = strtoull(output, NULL, 10) / (1024 * 1024 * 1024);
        free(output);
        format_row("Memory", "Available");
        printf("%llu GiB\n", total_mem);
        return;
    }
}
