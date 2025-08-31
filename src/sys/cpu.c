#include "cpu.h"
#include "types.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static double get_cpu_frequency(void)
{
    double freq = 0.0;

    // Try /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq first (most reliable)
    FILE *fp = fopen("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", "r");
    if (fp)
    {
        unsigned long khz;
        if (fscanf(fp, "%lu", &khz) == 1)
        {
            freq = khz / 1000.0; // Convert kHz to MHz
        }
        fclose(fp);
        if (freq > 0.0)
            return freq;
    }

    // Fallback: try /proc/cpuinfo
    fp = fopen("/proc/cpuinfo", "r");
    if (fp)
    {
        char line[256];
        while (fgets(line, sizeof(line), fp))
        {
            if (strncmp(line, "cpu MHz\t\t:", 11) == 0)
            {
                freq = atof(line + 11);
                if (freq > 0.0)
                    break;
            }
        }
        fclose(fp);
    }

    return freq;
}

void collect_cpu_info(void)
{
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (!fp)
        return;

    char line[256];
    int cores = 0;
    int model_found = 0;

    while (fgets(line, sizeof(line), fp))
    {
        if (!model_found && strncmp(line, "model name\t:", 12) == 0)
        {
            char *model = line + 13; // Skip "model name\t: "
            while (*model == ' ')
                model++; // Skip spaces

            size_t len = strlen(model);
            if (len > 0 && model[len - 1] == '\n')
                model[len - 1] = '\0';

            strncpy(g_system_info.cpu.model, model, sizeof(g_system_info.cpu.model) - 1);
            model_found = 1;
        }
        else if (strncmp(line, "processor\t:", 11) == 0)
        {
            cores++;
        }
    }
    fclose(fp);

    g_system_info.cpu.cores = cores;
    g_system_info.cpu.freq_mhz = get_cpu_frequency();
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