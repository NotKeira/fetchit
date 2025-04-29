#include "gpu.h"
#include "types.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

void collect_gpu_info(void)
{
    // Try reading from /sys/class/drm first
    DIR *dir = opendir("/sys/class/drm");
    if (dir)
    {
        struct dirent *entry;
        while ((entry = readdir(dir)))
        {
            if (strstr(entry->d_name, "card"))
            {
                char path[512];
                snprintf(path, sizeof(path), "/sys/class/drm/%s/device/uevent", entry->d_name);

                FILE *fp = fopen(path, "r");
                if (fp)
                {
                    char line[256];
                    while (fgets(line, sizeof(line), fp))
                    {
                        if (strstr(line, "DRIVER="))
                        {
                            char *driver = strchr(line, '=');
                            if (driver)
                            {
                                driver++;
                                size_t len = strlen(driver);
                                if (len > 0 && driver[len - 1] == '\n')
                                    driver[len - 1] = '\0';
                                snprintf(g_system_info.gpu.model, sizeof(g_system_info.gpu.model),
                                         "GPU Driver: %s", driver);
                            }
                            break;
                        }
                    }
                    fclose(fp);
                    break;
                }
            }
        }
        closedir(dir);
    }
}

void gpu_info(void)
{
    if (g_system_info.gpu.model[0] != '\0')
    {
        format_row("GPU", g_system_info.gpu.model);
    }
}
