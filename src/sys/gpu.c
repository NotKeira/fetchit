#include "gpu.h"
#include "types.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <glob.h>
#include <unistd.h>

void collect_gpu_info(void)
{
    // Use glob instead of directory scanning - much faster
    glob_t glob_result;

    // Find GPU cards directly with pattern matching
    if (glob("/sys/class/drm/card*/device/uevent", GLOB_NOSORT, NULL, &glob_result) == 0)
    {

        // Process first GPU only (early exit)
        if (glob_result.gl_pathc > 0)
        {
            FILE *fp = fopen(glob_result.gl_pathv[0], "r");
            if (fp)
            {
                char line[256];
                while (fgets(line, sizeof(line), fp))
                {
                    if (strncmp(line, "DRIVER=", 7) == 0)
                    {
                        char *driver = line + 7;
                        size_t len = strlen(driver);
                        if (len > 0 && driver[len - 1] == '\n')
                            driver[len - 1] = '\0';
                        snprintf(g_system_info.gpu.model, sizeof(g_system_info.gpu.model),
                                 "GPU Driver: %s", driver);
                        break;
                    }
                }
                fclose(fp);
            }
        }
        globfree(&glob_result);
    }

    // Fallback: try common GPU info files directly
    if (g_system_info.gpu.model[0] == '\0')
    {
        // Try reading GPU info from direct paths
        const char *gpu_paths[] = {
            "/sys/class/drm/card0/device/uevent",
            "/sys/class/drm/card1/device/uevent",
            NULL};

        for (int i = 0; gpu_paths[i] != NULL; i++)
        {
            if (access(gpu_paths[i], R_OK) == 0)
            {
                FILE *fp = fopen(gpu_paths[i], "r");
                if (fp)
                {
                    char line[256];
                    while (fgets(line, sizeof(line), fp))
                    {
                        if (strncmp(line, "DRIVER=", 7) == 0)
                        {
                            char *driver = line + 7;
                            size_t len = strlen(driver);
                            if (len > 0 && driver[len - 1] == '\n')
                                driver[len - 1] = '\0';
                            snprintf(g_system_info.gpu.model, sizeof(g_system_info.gpu.model),
                                     "GPU Driver: %s", driver);
                            fclose(fp);
                            return;
                        }
                    }
                    fclose(fp);
                }
            }
        }
    }
}

void gpu_info(void)
{
    if (g_system_info.gpu.model[0] != '\0')
    {
        format_row("GPU", g_system_info.gpu.model);
    }
}