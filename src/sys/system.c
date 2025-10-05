/**
 * system.c - System Information Collection
 *
 * Retrieves basic system information including hostname, kernel version,
 * architecture, operating system, and installed package count. Uses
 * optimised methods for efficient data collection.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <sys/stat.h>
#include "system.h"
#include "types.h"
#include "utils.h"

/**
 * count_packages_fast - Count installed Debian packages
 *
 * Parses /var/lib/dpkg/status to count installed packages. Uses
 * single-file parsing instead of directory scanning for improved
 * performance. Only counts packages with "install ok installed"
 * status.
 *
 * Return: Number of installed packages, or 0 on error
 */
static int count_packages_fast(void)
{
    FILE *fp = fopen("/var/lib/dpkg/status", "r");
    if (!fp)
        return 0;

    char line[256];
    int count = 0;
    int in_package = 0;
    int is_installed = 0;

    while (fgets(line, sizeof(line), fp))
    {
        if (strncmp(line, "Package:", 8) == 0)
        {
            in_package = 1;
            is_installed = 0;
        }
        else if (in_package && strncmp(line, "Status:", 7) == 0)
        {
            if (strstr(line, "install ok installed"))
            {
                is_installed = 1;
            }
        }
        else if (in_package && line[0] == '\n')
        {
            /* End of package block */
            if (is_installed)
                count++;
            in_package = 0;
        }
    }

    fclose(fp);
    return count;
}

/**
 * collect_system_info - Gather basic system information
 *
 * Retrieves hostname, kernel version, and architecture via uname().
 * Reads operating system name from /etc/os-release and counts
 * installed packages using count_packages_fast(). Populates the
 * global system_info structure with collected data.
 */
void collect_system_info(void)
{
    struct utsname uts;
    if (uname(&uts) < 0)
        return;

    strncpy(g_system_info.system.hostname, uts.nodename, sizeof(g_system_info.system.hostname) - 1);
    strncpy(g_system_info.system.kernel, uts.release, sizeof(g_system_info.system.kernel) - 1);
    strncpy(g_system_info.system.arch, uts.machine, sizeof(g_system_info.system.arch) - 1);

    /* Read OS distribution name from /etc/os-release */
    FILE *fp = fopen("/etc/os-release", "r");
    if (fp)
    {
        char line[256];
        while (fgets(line, sizeof(line), fp))
        {
            if (strncmp(line, "PRETTY_NAME=", 12) == 0)
            {
                char *name = strchr(line, '\"');
                if (name)
                {
                    name++;
                    char *end = strchr(name, '\"');
                    if (end)
                        *end = '\0';
                    strncpy(g_system_info.system.os, name, sizeof(g_system_info.system.os) - 1);
                }
                break;
            }
        }
        fclose(fp);
    }

    /* Count installed packages using optimised method */
    g_system_info.system.package_count = count_packages_fast();
}