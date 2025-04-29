#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "utils.h"
#include "types.h"
#include "system.h"
#include "memory.h"
#include "cpu.h"
#include "gpu.h"
#include "storage.h"

system_info_t g_system_info = {0};

typedef struct
{
    void (*collect_fn)(void);
} collector_t;

static collector_t collectors[] = {
    {collect_system_info},
    {collect_cpu_info},
    {collect_gpu_info},
    {collect_memory_info},
    {collect_storage_info}};

static void *collector_thread(void *arg)
{
    collector_t *collector = (collector_t *)arg;
    collector->collect_fn();
    return NULL;
}

static void print_all_info(void)
{
    format_row("Hostname", g_system_info.system.hostname);
    format_row("OS", g_system_info.system.os);
    format_row("Kernel", g_system_info.system.kernel);
    format_row("Architecture", g_system_info.system.arch);

    char pkg_count[32];
    snprintf(pkg_count, sizeof(pkg_count), "%d", g_system_info.system.package_count);
    format_row("Packages", pkg_count);

    cpu_info();
    gpu_info();
    memory_info();
    storage_info();
}

int main()
{
    pthread_t threads[5];
    print_title("==== FetchIt ====\n");

    // Start all collectors
    for (size_t i = 0; i < sizeof(collectors) / sizeof(collectors[0]); i++)
    {
        pthread_create(&threads[i], NULL, collector_thread, &collectors[i]);
    }

    // Wait for all collectors
    for (size_t i = 0; i < sizeof(collectors) / sizeof(collectors[0]); i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Print everything in the correct order
    print_all_info();

    return 0;
}
