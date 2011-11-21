#include "resource.h"
#include <iostream>

int main() {
    cpu_percent_t *per = cpu_percent_usage();
    mem_stat_t *mem = get_memory_stats();

    std::cout << per->user << std::endl;
    std::cout << mem->used << std::endl;
    return 0;
}
