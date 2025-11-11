#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cache.h"

void print_usage() {
    fprintf(stderr,
            "Usage: ./cache_simulator <cache_size> <block_size> "
            "<associativity> <policy> <trace_file>\n");
    fprintf(stderr, "\n");
    fprintf(stderr,
            "  <cache_size>:    Total cache size in bytes (e.g., 1024)\n");
    fprintf(stderr, "  <block_size>:    Block size in bytes (e.g., 64)\n");
    fprintf(stderr,
            "  <associativity>: N-way associativity (1 for direct-mapped, 0 "
            "for fully-associative)\n");
    fprintf(stderr,
            "  <policy>:        Replacement policy ('LRU' or 'FIFO')\n");
    fprintf(stderr, "  <trace_file>:    Path to the memory trace file\n");
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        print_usage();
        return 1;
    }

    CacheConfig config;
    config.cache_size = (uint32_t)atoi(argv[1]);
    config.block_size = (uint32_t)atoi(argv[2]);
    config.associativity = (uint32_t)atoi(argv[3]);
    strncpy(config.policy, argv[4], 4);
    config.policy[4] = '\0';  // Ensure null-termination

    char* trace_file_name = argv[5];

    // Input validation
    if (strcmp(config.policy, "LRU") != 0 &&
        strcmp(config.policy, "FIFO") != 0) {
        fprintf(stderr,
                "Error: Invalid replacement policy. Use 'LRU' or 'FIFO'.\n");
        return 1;
    }

    // Note: Add more validation (e.g., check for powers of 2) if needed

    Cache* cache = cache_create(config);
    if (!cache) {
        fprintf(stderr, "Error: Could not create cache.\n");
        return 1;
    }

    FILE* trace_file = fopen(trace_file_name, "r");
    if (!trace_file) {
        fprintf(stderr, "Error: Could not open trace file '%s'.\n",
                trace_file_name);
        cache_destroy(cache);
        return 1;
    }

    char line_buffer[100];
    unsigned long long address;  // Use long long for sscanf
    char type;                   // To handle traces like "L 0x..."

    while (fgets(line_buffer, sizeof(line_buffer), trace_file)) {
        // Support two common trace formats:
        // 1. "0x[address]"
        // 2. "[type] 0x[address]" (e.g., "L 0x123", "S 0x456")

        if (sscanf(line_buffer, " %c 0x%llx", &type, &address) == 2) {
            // Format 2: "L 0x..."
            cache_access(cache, (uint64_t)address);
        } else if (sscanf(line_buffer, "0x%llx", &address) == 1) {
            // Format 1: "0x..."
            cache_access(cache, (uint64_t)address);
        }
    }

    fclose(trace_file);
    print_stats(cache);
    cache_destroy(cache);

    return 0;
}