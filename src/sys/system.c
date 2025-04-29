#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <dirent.h>
#include "system.h"
#include "types.h"
#include "utils.h"

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

    // Count packages in /var/lib/dpkg/info (Debian-based)
    DIR *d = opendir("/var/lib/dpkg/info");
    if (d)
    {
        struct dirent *dir;
        while ((dir = readdir(d)) != NULL)
        {
            if (strstr(dir->d_name, ".list"))
            {
                g_system_info.system.package_count++;
            }
        }
        closedir(d);
    }
}

void system_info(void)
{
    format_row("Hostname", g_system_info.system.hostname);
    format_row("OS", g_system_info.system.os);
    format_row("Kernel", g_system_info.system.kernel);
    format_row("Architecture", g_system_info.system.arch);

    char pkg_count[32];
    snprintf(pkg_count, sizeof(pkg_count), "%d", g_system_info.system.package_count);
    format_row("Packages", pkg_count);
}
