/**
 * storage.c - Storage Information Collection
 *
 * Retrieves filesystem information from mounted volumes, filtering
 * for relevant physical and user filesystems. Excludes virtual and
 * system filesystems to present only meaningful storage data.
 */

#include "storage.h"
#include "types.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <sys/statvfs.h>
#include <mntent.h>

#define MAX_FILESYSTEMS 16

typedef struct
{
    char mount_point[256];
    char device[256];
    double total_gb;
    double used_gb;
    double free_gb;
} filesystem_info_t;

static filesystem_info_t filesystems[MAX_FILESYSTEMS];
static int filesystem_count = 0;

/**
 * is_important_filesystem - Determine if filesystem should be displayed
 * @mount_point: Mount path of the filesystem
 * @fstype: Filesystem type string
 *
 * Filters filesystems to include only physical storage and user-mounted
 * volumes. Excludes virtual/pseudo filesystems and system directories.
 *
 * Return: 1 if filesystem is relevant, 0 otherwise
 */
static int is_important_filesystem(const char *mount_point, const char *fstype)
{
    /* Always include root filesystem */
    if (strcmp(mount_point, "/") == 0)
        return 1;

    /* Include /home if mounted separately */
    if (strcmp(mount_point, "/home") == 0)
        return 1;

    /* Exclude virtual/pseudo filesystems */
    if (strncmp(fstype, "proc", 4) == 0 ||
        strncmp(fstype, "sys", 3) == 0 ||
        strncmp(fstype, "dev", 3) == 0 ||
        strncmp(fstype, "run", 3) == 0 ||
        strncmp(fstype, "tmp", 3) == 0 ||
        strcmp(fstype, "cgroup") == 0 ||
        strcmp(fstype, "cgroup2") == 0 ||
        strcmp(fstype, "configfs") == 0 ||
        strcmp(fstype, "debugfs") == 0 ||
        strcmp(fstype, "securityfs") == 0 ||
        strcmp(fstype, "pstore") == 0 ||
        strcmp(fstype, "efivarfs") == 0 ||
        strcmp(fstype, "bpf") == 0 ||
        strcmp(fstype, "overlay") == 0)
    {
        return 0;
    }

    /* Include user-mounted filesystems under /media, /mnt, or /home subdirectories */
    if (strncmp(mount_point, "/media/", 7) == 0 ||
        strncmp(mount_point, "/mnt/", 5) == 0 ||
        (strncmp(mount_point, "/home/", 6) == 0 && strlen(mount_point) > 6))
    {
        return 1;
    }

    /* Include recognised physical filesystem types outside system directories */
    if (mount_point[0] == '/' &&
        strncmp(mount_point, "/proc", 5) != 0 &&
        strncmp(mount_point, "/sys", 4) != 0 &&
        strncmp(mount_point, "/dev", 4) != 0 &&
        strncmp(mount_point, "/run", 4) != 0 &&
        strncmp(mount_point, "/tmp", 4) != 0 &&
        (strcmp(fstype, "ext4") == 0 ||
         strcmp(fstype, "ext3") == 0 ||
         strcmp(fstype, "ext2") == 0 ||
         strcmp(fstype, "btrfs") == 0 ||
         strcmp(fstype, "xfs") == 0 ||
         strcmp(fstype, "zfs") == 0 ||
         strcmp(fstype, "ntfs") == 0 ||
         strcmp(fstype, "vfat") == 0 ||
         strcmp(fstype, "exfat") == 0))
    {
        return 1;
    }

    return 0;
}

/**
 * collect_storage_info - Gather filesystem statistics
 *
 * Parses /proc/mounts to enumerate mounted filesystems, filtering
 * for relevant volumes via is_important_filesystem(). Retrieves
 * capacity and usage statistics using statvfs() for each qualifying
 * filesystem. Stores results in the static filesystems array.
 */
void collect_storage_info(void)
{
    FILE *fp = fopen("/proc/mounts", "r");
    if (!fp)
        return;

    filesystem_count = 0;
    static const double bytes_to_gb = 1.0 / (1024.0 * 1024.0 * 1024.0);

    struct mntent *entry;
    while ((entry = getmntent(fp)) && filesystem_count < MAX_FILESYSTEMS)
    {
        if (!is_important_filesystem(entry->mnt_dir, entry->mnt_type))
        {
            continue;
        }

        struct statvfs buf;
        if (statvfs(entry->mnt_dir, &buf) != 0)
        {
            continue;
        }

        /* Skip filesystems with no blocks (virtual filesystems) */
        if (buf.f_blocks == 0)
            continue;

        filesystem_info_t *fs = &filesystems[filesystem_count];

        strncpy(fs->mount_point, entry->mnt_dir, sizeof(fs->mount_point) - 1);
        strncpy(fs->device, entry->mnt_fsname, sizeof(fs->device) - 1);

        double total_bytes = (double)(buf.f_blocks * buf.f_frsize);
        double used_bytes = (double)((buf.f_blocks - buf.f_bfree) * buf.f_frsize);

        fs->total_gb = total_bytes * bytes_to_gb;
        fs->used_gb = used_bytes * bytes_to_gb;
        fs->free_gb = fs->total_gb - fs->used_gb;

        filesystem_count++;
    }

    fclose(fp);
}

/**
 * storage_info - Display formatted storage information
 *
 * Prints capacity, usage, and percentage utilisation for each
 * collected filesystem. Formats mount points in the display key
 * for clarity. Outputs error message if no filesystems were found.
 */
void storage_info(void)
{
    if (filesystem_count == 0)
    {
        format_row("Storage", "Error retrieving data");
        return;
    }

    for (int i = 0; i < filesystem_count; i++)
    {
        filesystem_info_t *fs = &filesystems[i];

        double usage_percent = (fs->used_gb / fs->total_gb) * 100.0;

        char storage_info[512];
        char key[64];

        /* Format key based on mount point significance */
        if (strcmp(fs->mount_point, "/") == 0)
        {
            snprintf(key, sizeof(key), "Storage (/)");
        }
        else if (strcmp(fs->mount_point, "/home") == 0)
        {
            snprintf(key, sizeof(key), "Storage (/home)");
        }
        else
        {
            snprintf(key, sizeof(key), "Storage (%s)", fs->mount_point);
        }

        snprintf(storage_info, sizeof(storage_info),
                 "%.1f GiB / %.1f GiB (%.1f%% used)",
                 fs->used_gb, fs->total_gb, usage_percent);

        format_row(key, storage_info);
    }
}