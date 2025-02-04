#include "cpu.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void cpu_info()
{
    char *output = get_command_output("lscpu | grep 'Model name' | cut -d ':' -f2 | sed 's/^ *//'");
    if (output != NULL)
    {
        format_row("CPU", output);
        free(output);
    }
    else
    {
        format_row("CPU", "Not detected");
    }
}
