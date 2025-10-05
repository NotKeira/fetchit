/**
 * cpu.c - CPU Information Collection
 *
 * Retrieves processor information from /proc/cpuinfo and sysfs,
 * including model name, core count, and operating frequency.
 */

#include "cpu.h"
#include "types.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * get_cpu_frequency - Determine CPU operating frequency
 *
 * Attempts to read the CPU frequency from sysfs (most reliable method),
 * falling back to /proc/cpuinfo if sysfs data is unavailable.
 *
 * Return: CPU frequency in MHz, or 0.0 if unable to determine
 */
static double get_cpu_frequency(void)
{
    double freq = 0.0;

    /* Primary method: read max frequency from sysfs */
    FILE *fp = fopen("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", "r");
    if (fp)
    {
        unsigned long khz;
        if (fscanf(fp, "%lu", &khz) == 1)
        {
            freq = khz / 1000.0; /* Convert kHz to MHz */
        }
        fclose(fp);
        if (freq > 0.0)
            return freq;
    }

    /* Fallback: parse /proc/cpuinfo for current frequency */
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

/**
 * collect_cpu_info - Gather CPU information
 *
 * Parses /proc/cpuinfo to extract the processor model name and
 * count logical cores. Retrieves frequency separately via
 * get_cpu_frequency(). Populates the global system_info structure.
 */
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
        /* Extract CPU model name from first occurrence */
        if (!model_found && strncmp(line, "model name\t:", 12) == 0)
        {
            char *model = line + 13; /* Skip "model name\t: " */

            /* Trim leading whitespace */
            while (*model == ' ')
                model++;

            /* Remove trailing newline */
            size_t len = strlen(model);
            if (len > 0 && model[len - 1] == '\n')
                model[len - 1] = '\0';

            strncpy(g_system_info.cpu.model, model, sizeof(g_system_info.cpu.model) - 1);
            model_found = 1;
        }
        /* Count logical processors */
        else if (strncmp(line, "processor\t:", 11) == 0)
        {
            cores++;
        }
    }

    fclose(fp);

    g_system_info.cpu.cores = cores;
    g_system_info.cpu.freq_mhz = get_cpu_frequency();
}

/**
 * cpu_info - Display formatted CPU information
 *
 * Prints the CPU model, core count, and frequency in a
 * human-readable format.
 */
void cpu_info(void)
{
    char cpu_info[512];
    snprintf(cpu_info, sizeof(cpu_info), "%s (%d cores @ %.0f MHz)",
             g_system_info.cpu.model,
             g_system_info.cpu.cores,
             g_system_info.cpu.freq_mhz);

    format_row("CPU", cpu_info);
}