#include "cpu.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CPU_INFO_FILE "/proc/cpuinfo"

static char* get_cpu_model() {
    FILE* fp = fopen(CPU_INFO_FILE, "r");
    if (!fp) return NULL;

    char line[256];
    char* model = NULL;
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "model name", 10) == 0) {
            char* colon = strchr(line, ':');
            if (colon) {
                model = strdup(colon + 2);
                if (model) {
                    char* newline = strchr(model, '\n');
                    if (newline) *newline = '\0';
                }
                break;
            }
        }
    }
    fclose(fp);
    return model;
}
void cpu_info()
{
    char *output = get_cpu_model();
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
