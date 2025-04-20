#include "memory.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
