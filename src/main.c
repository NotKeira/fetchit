/**
 * FetchIt - System Information Display Tool
 *
 * Main entry point for the system information fetching utility.
 * Collects and displays hardware and software information in a
 * formatted output.
 */

#include <stdio.h>
#include <unistd.h>
#include "utils.h"
#include "types.h"
#include "system.h"
#include "memory.h"
#include "cpu.h"
#include "gpu.h"
#include "storage.h"

/* Global system information structure */
system_info_t g_system_info = {0};

/**
 * collect_all_info - Gather all system information
 *
 * Sequentially collects information from all subsystems and populates
 * the global system_info structure. Collection order ensures dependencies
 * are resolved correctly.
 */
static inline void collect_all_info(void)
{
    collect_system_info();
    collect_cpu_info();
    collect_gpu_info();
    collect_memory_info();
    collect_storage_info();
}

/**
 * print_all_info - Display all collected system information
 *
 * Formats and prints system information in a consistent layout.
 * Basic system details are displayed first, followed by hardware
 * component information from specialised functions.
 */
static inline void print_all_info(void)
{
    format_row("Hostname", g_system_info.system.hostname);
    format_row("OS", g_system_info.system.os);
    format_row("Kernel", g_system_info.system.kernel);
    format_row("Architecture", g_system_info.system.arch);

    char pkg_count[32];
    snprintf(pkg_count, sizeof(pkg_count), "%d", g_system_info.system.package_count);
    format_row("Packages", pkg_count);

    cpu_info();
    gpu_info();
    memory_info();
    storage_info();
}

/**
 * main - Programme entry point
 *
 * Initialises the application, collects system information, and
 * displays the formatted output.
 *
 * Return: 0 on success
 */
int main()
{
    print_title("==== FetchIt ====\n");

    collect_all_info();
    print_all_info();

    return 0;
}