#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <stdbool.h>
#include "system.h"
#include "utils.h"
#include "colour.h"

static bool is_command_available(const char *command)
{
    char check_command[256];
    snprintf(check_command, sizeof(check_command), "command -v %s > /dev/null 2>&1", command);
    return system(check_command) == 0;
}

void system_info()
{
    retrieve_system_info();
    retrieve_kernel_info();
    retrieve_package_info();
}

void retrieve_system_info()
{
    struct utsname buffer;
    if (uname(&buffer) != 0)
    {
        perror("uname");
        return;
    }

    char *os_info = get_command_output("cat /etc/os-release | grep PRETTY_NAME | cut -d '\"' -f2");
    if (os_info)
    {
        format_row("OS", os_info);
        free(os_info);
    }
    else
    {
        format_row("OS", "Unknown");
    }

    format_row("Assembly", buffer.machine);
    format_row("Hostname", buffer.nodename);
}

void retrieve_kernel_info()
{
    struct utsname buffer;
    if (uname(&buffer) != 0)
    {
        perror("uname");
        return;
    }

    char kernel_info[256];
    snprintf(kernel_info, sizeof(kernel_info), "%s %s", buffer.sysname, buffer.release);
    format_row("Kernel", kernel_info);
    format_row("Version", buffer.version);
}

void retrieve_package_info()
{
    if (is_command_available("dnf"))
    {
        char *output = get_command_output("dnf list installed | wc -l");
        if (output)
        {
            format_row("Packages (DNF)", output);
            free(output);
            return;
        }
    }
    else if (is_command_available("dpkg"))
    {
        char *output = get_command_output("dpkg -l | wc -l");
        if (output)
        {
            format_row("Packages (APT)", output);
            free(output);
            return;
        }
    }
    else if (is_command_available("snap"))
    {
        char *output = get_command_output("snap list | wc -l");
        if (output)
        {
            format_row("Packages (Snap)", output);
            free(output);
            return;
        }
    }

    format_row("Packages", "Not Available");
}
