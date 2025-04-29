#ifndef TYPES_H
#define TYPES_H

typedef struct
{
    char os[256];
    char kernel[256];
    char hostname[256];
    char arch[64];
    int package_count;
} system_data_t;

typedef struct
{
    char model[256];
    int cores;
    double freq_mhz;
} cpu_data_t;

typedef struct
{
    char model[256];
    unsigned long vram_mb;
} gpu_data_t;

typedef struct
{
    unsigned long total_kb;
    unsigned long used_kb;
    unsigned long free_kb;
} memory_data_t;

typedef struct
{
    double total_gb;
    double used_gb;
    double free_gb;
} storage_data_t;

typedef struct
{
    int ready;
    system_data_t system;
    cpu_data_t cpu;
    gpu_data_t gpu;
    memory_data_t memory;
    storage_data_t storage;
} system_info_t;

extern system_info_t g_system_info;

#endif // TYPES_H
