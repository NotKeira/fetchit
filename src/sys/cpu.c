/**
 * cpu.c - CPU Information Collection
 *
 * Retrieves processor information including model name, core count, and operating frequency.
 */

#include "cpu.h"
#include "types.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#include <powerbase.h>

#pragma warning(push)
#pragma warning(disable: 4005)

#include <ntstatus.h>

#pragma warning(pop)

#pragma comment(lib, "PowrProf.lib")
#pragma comment(lib, "Advapi32.lib")

typedef struct _PROCESSOR_POWER_INFORMATION {
    ULONG Number;
    ULONG MaxMhz;
    ULONG CurrentMhz;
    ULONG MhzLimit;
    ULONG MaxIdleState;
    ULONG CurrentIdleState;
} PROCESSOR_POWER_INFORMATION, *PPROCESSOR_POWER_INFORMATION;
#else
#include <string.h>
#include <unistd.h>
#endif

/**
 * get_cpu_frequency - Determine CPU operating frequency
 *
 * On Windows, it attempts to read the CPU frequency by calling NT power processor information
 * falling back to the registry if unable to.
 *
 * On Linux, it attempts to read the CPU frequency from sysfs (most reliable method),
 * falling back to /proc/cpuinfo if sysfs data is unavailable.
 *
 * Return: CPU frequency in MHz, or 0.0 if unable to determine
 */
static double get_cpu_frequency(void)
{
    double freq = 0.0;

#ifdef _WIN32
    PROCESSOR_POWER_INFORMATION processor_power_info;

    if (CallNtPowerInformation(ProcessorInformation, NULL, 0, &processor_power_info, sizeof(PROCESSOR_POWER_INFORMATION)) == STATUS_SUCCESS) {
        freq = (double)processor_power_info.CurrentMhz;
    } else {
        DWORD approximate_cpu_mhz;
        DWORD approximate_cpu_mhz_size = sizeof(DWORD);

        if (RegGetValueA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", "~MHz", RRF_RT_DWORD, NULL, (LPDWORD)&approximate_cpu_mhz, &approximate_cpu_mhz_size) == ERROR_SUCCESS) {
            freq = (double)approximate_cpu_mhz;
        }
    }
#else
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
#endif

    return freq;
}

/**
 * collect_cpu_info - Gather CPU information
 *
 * On Windows, it retrieves relevant CPU information from the registry and other win32 APIs.
 *
 * On Linux, it parses /proc/cpuinfo to extract the processor model name and
 * count logical cores.
 *
 * CPU frequency is retrieved separately via
 * get_cpu_frequency().
 *
 * And ultimately, the function populates the global system_info structure.
 */
void collect_cpu_info(void)
{
    int cores = 0;

#ifdef _WIN32
    DWORD cpu_model_buffer_size = 256;

    RegGetValueA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", "ProcessorNameString", RRF_RT_REG_SZ, NULL, (PVOID)g_system_info.cpu.model, &cpu_model_buffer_size);

    DWORD processor_info_buffer_size = 0;

    if (GetLogicalProcessorInformationEx(RelationProcessorCore, NULL, &processor_info_buffer_size) == FALSE && GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
        return;
    }

    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX processor_info = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)malloc(processor_info_buffer_size);

    if (processor_info != NULL) {
        if (GetLogicalProcessorInformationEx(RelationProcessorCore, processor_info, &processor_info_buffer_size) == TRUE) {
            char* ptr = (char*)processor_info;
            char* end = ptr + processor_info_buffer_size;

            while (ptr < end) {
                PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX current_processor_info = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)ptr;

                if (current_processor_info->Relationship == RelationProcessorCore) {
                    cores++;
                }

                ptr += current_processor_info->Size;
            }
        }

        free(processor_info);
    }
#else
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (!fp)
        return;

    char line[256];
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
#endif

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