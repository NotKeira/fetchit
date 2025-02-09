#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include "system.h"
#include "utils.h"
#include "colour.h"

static void retrieve_system_info();
static void retrieve_package_info();
static void retrieve_kernel_info();

/**
 * Prints all general system information in a single function call.
 */
void system_info()
{
    print_title("System Information");
    retrieve_system_info();
    retrieve_kernel_info();
    retrieve_package_info();
}

/**
 * Retrieves basic system details like machine type, hostname, and OS version.
 */
static void retrieve_system_info()
{
    struct utsname buffer;
    if (uname(&buffer) != 0)
    {
        perror("uname");
        return;
    }

    format_row("Machine", buffer.machine);
    format_row("Hostname", buffer.nodename);
}

/**
 * Retrieves kernel-related information.
 */
static void retrieve_kernel_info()
{
    struct utsname buffer;
    if (uname(&buffer) != 0)
    {
        perror("uname");
        return;
    }

    format_row("Kernel", buffer.sysname);
    format_row("Release", buffer.release);
    format_row("Version", buffer.version);
}

/**
 * Retrieves package count from different package managers.
 */
static void retrieve_package_info()
{
    char *output = get_command_output("dnf list installed | wc -l");
    if (output != NULL)
    {
        format_row("Packages (DNF)", output);
        free(output);
        return;
    }

    output = get_command_output("dpkg -l | wc -l");
    if (output != NULL)
    {
        format_row("Packages (APT)", output);
        free(output);
        return;
    }

    output = get_command_output("snap list | wc -l");
    if (output != NULL)
    {
        format_row("Packages (Snap)", output);
        free(output);
        return;
    }

    format_row("Packages", "Not Available");
}
