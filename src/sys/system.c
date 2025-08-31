#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <sys/stat.h>
#include "system.h"
#include "types.h"
#include "utils.h"

static int count_packages_fast(void)
{
    // Read /var/lib/dpkg/status (single file, faster than directory scan)
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
            // End of package block
            if (is_installed)
                count++;
            in_package = 0;
        }
    }
    fclose(fp);
    return count;
}

void collect_system_info(void)
{
    struct utsname uts;
    if (uname(&uts) < 0)
        return;

    strncpy(g_system_info.system.hostname, uts.nodename, sizeof(g_system_info.system.hostname) - 1);
    strncpy(g_system_info.system.kernel, uts.release, sizeof(g_system_info.system.kernel) - 1);
    strncpy(g_system_info.system.arch, uts.machine, sizeof(g_system_info.system.arch) - 1);

    // Read OS info from /etc/os-release
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

    // Use optimised package counting
    g_system_info.system.package_count = count_packages_fast();
}