/**
 * gpu.c - GPU Information Collection
 *
 * Retrieves graphics card information from sysfs DRM subsystem.
 * Uses pattern matching for efficient device discovery and extracts
 * driver information from uevent files.
 */

#include "gpu.h"
#include "types.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <glob.h>
#include <unistd.h>

/**
 * collect_gpu_info - Gather GPU information
 *
 * Attempts to identify the GPU driver by reading DRM card device
 * information from sysfs. Uses glob for efficient pattern-based
 * device discovery, falling back to direct path access if needed.
 * Only processes the first detected GPU card.
 *
 * The function populates g_system_info.gpu.model with the driver
 * name if found, otherwise leaves it empty.
 */
void collect_gpu_info(void)
{
    glob_t glob_result;

    /* Primary method: use glob pattern matching for fast discovery */
    if (glob("/sys/class/drm/card*/device/uevent", GLOB_NOSORT, NULL, &glob_result) == 0)
    {
        /* Process first GPU only for performance */
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

                        /* Remove trailing newline */
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

    /* Fallback: attempt direct path access to common card devices */
    if (g_system_info.gpu.model[0] == '\0')
    {
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

                            /* Remove trailing newline */
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

/**
 * gpu_info - Display formatted GPU information
 *
 * Prints GPU driver information if available. Omits output
 * if no GPU information was collected.
 */
void gpu_info(void)
{
    if (g_system_info.gpu.model[0] != '\0')
    {
        format_row("GPU", g_system_info.gpu.model);
    }
}