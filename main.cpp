#include "resource.h"
#include <iostream>

int main() {
    cpu_percent_t *per = cpu_percent_usage();
    std::cout << per->user;
    return 0;
}
