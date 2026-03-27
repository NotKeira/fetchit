/**
 * system.c - System Information Collection
 *
 * Retrieves basic system information including hostname, kernel version,
 * architecture, operating system, and installed package count. Uses
 * optimised methods for efficient data collection.
 */

#include "system.h"
#include "types.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>

#pragma warning(push)
#pragma warning(disable: 4005)

#include <ntstatus.h>

#pragma warning(pop)

#pragma comment(lib, "Advapi32.lib")

typedef NTSTATUS (WINAPI* RTL_GET_VERSION)(PRTL_OSVERSIONINFOW);

static DWORD count_registry_subkeys(HKEY base_key, LPCSTR subkey) {
    HKEY registry_key;
    DWORD count = 0;

    if (RegOpenKeyExA(base_key, subkey, 0, KEY_READ, &registry_key) == ERROR_SUCCESS) {
        RegQueryInfoKeyA(registry_key, NULL, NULL, NULL, &count, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        RegCloseKey(registry_key);
    }

    return count;
}
#else
#include <stdlib.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <sys/stat.h>
#endif

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
static int count_packages_fast(void) {
    int count = 0;

#ifdef _WIN32
    count += (int)count_registry_subkeys(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
    count += (int)count_registry_subkeys(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
#else
    FILE *fp = fopen("/var/lib/dpkg/status", "r");
    if (!fp)
        return 0;

    char line[256];
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
#endif

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
#ifdef _WIN32
    DWORD hostname_buffer_size = ARRAYSIZE(g_system_info.system.hostname);

    GetComputerNameExA(ComputerNameDnsHostname, g_system_info.system.hostname, &hostname_buffer_size);

    HMODULE ntdll = GetModuleHandleA("ntdll.dll");

    if (ntdll != NULL && ntdll != INVALID_HANDLE_VALUE) {
        RTL_GET_VERSION rtl_get_version = (RTL_GET_VERSION)GetProcAddress(ntdll, "RtlGetVersion");

        if (rtl_get_version != NULL) {
            RTL_OSVERSIONINFOW os_version_info;

            os_version_info.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);

            if (rtl_get_version(&os_version_info) == STATUS_SUCCESS) {
                snprintf(g_system_info.system.kernel, ARRAYSIZE(g_system_info.system.kernel), "Windows %u.%u (Build %u)", os_version_info.dwMajorVersion, os_version_info.dwMinorVersion, os_version_info.dwBuildNumber);
            }
        }
    }

    SYSTEM_INFO system_info;

    GetNativeSystemInfo(&system_info);

    switch (system_info.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_AMD64: strcpy(g_system_info.system.arch, "x64"); break;
        case PROCESSOR_ARCHITECTURE_ARM: strcpy(g_system_info.system.arch, "arm"); break;
        case PROCESSOR_ARCHITECTURE_ARM64: strcpy(g_system_info.system.arch, "arm64"); break;
        case PROCESSOR_ARCHITECTURE_IA64: strcpy(g_system_info.system.arch, "ia64"); break;
        case PROCESSOR_ARCHITECTURE_INTEL: strcpy(g_system_info.system.arch, "x86"); break;
        default: strcpy(g_system_info.system.arch, "Unknown");
    }

    DWORD os_name_buffer_size = ARRAYSIZE(g_system_info.system.os);

    RegGetValueA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "ProductName", RRF_RT_REG_SZ, NULL, (PVOID)g_system_info.system.os, &os_name_buffer_size);
#else
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
#endif

    /* Count installed packages using optimised method */
    g_system_info.system.package_count = count_packages_fast();
}