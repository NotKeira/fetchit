#include "cpu.h"
#include "types.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void collect_cpu_info(void)
{
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (!fp)
        return;

    char line[256];
    int cores = 0;
    double freq = 0.0;

    while (fgets(line, sizeof(line), fp))
    {
        if (strstr(line, "model name"))
        {
            char *model = strchr(line, ':');
            if (model)
            {
                model++; // Skip colon
                while (*model == ' ')
                    model++; // Skip spaces
                size_t len = strlen(model);
                if (len > 0 && model[len - 1] == '\n')
                    model[len - 1] = '\0';
                strncpy(g_system_info.cpu.model, model, sizeof(g_system_info.cpu.model) - 1);
            }
        }
        else if (strstr(line, "processor"))
        {
            cores++;
        }
        else if (strstr(line, "cpu MHz"))
        {
            char *mhz = strchr(line, ':');
            if (mhz)
            {
                freq = atof(mhz + 1);
            }
        }
    }
    fclose(fp);

    g_system_info.cpu.cores = cores;
    g_system_info.cpu.freq_mhz = freq;
}

void cpu_info(void)
{
    char cpu_info[512];
    snprintf(cpu_info, sizeof(cpu_info), "%s (%d cores @ %.0f MHz)",
             g_system_info.cpu.model,
             g_system_info.cpu.cores,
             g_system_info.cpu.freq_mhz);
    format_row("CPU", cpu_info);
}
