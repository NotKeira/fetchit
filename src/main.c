#include <stdio.h>
#include "utils.h"
#include "colour.h"
#include "system.h"
#include "memory.h"
#include "cpu.h"
#include "gpu.h"
#include "storage.h"

int main()
{
    print_title("==== FetchIt ====");
    system_info();
    cpu_info();
    gpu_info();
    memory_info();
    storage_info();
    return 0;
}
