#include "gpu.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

void gpu_info()
{
    char *output = get_command_output("lspci | grep VGA | cut -d ' ' -f5-");
    if (output != NULL)
    {
        format_row("GPU", output);
        free(output);
    }
    else
    {
        format_row("GPU", "Not detected");
    }
}
