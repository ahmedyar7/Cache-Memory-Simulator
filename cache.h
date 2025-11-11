#ifndef CACHE_H
#define CACHE_H

#include <stdbool.h>
#include <stdint.h>

/*
 * Represents a single line in the cache.
 */
typedef struct {
    bool valid;
    uint64_t tag;
    uint32_t last_access_time;  // For LRU
    uint32_t arrival_time;      // For FIFO
} CacheLine;

/*
 * Holds the configuration parameters for the cache.
 */
typedef struct {
    uint32_t cache_size;     // Total size in bytes
    uint32_t block_size;     // Size of one block in bytes
    uint32_t associativity;  // N-way set associativity (1 for direct-mapped)
    char policy[5];          // "LRU" or "FIFO"
} CacheConfig;

/*
 * Holds the simulation statistics.
 */
typedef struct {
    uint32_t hits;
    uint32_t misses;
    uint32_t accesses;
} CacheStats;

/*
 * The main cache structure.
 */
typedef struct {
    CacheLine* lines;  // Pointer to the contiguous block of all cache lines
    CacheConfig config;
    CacheStats stats;

    // Derived parameters
    uint32_t num_sets;
    uint32_t set_bits;
    uint32_t block_bits;

    uint32_t current_time;  // Global logical clock for LRU/FIFO
} Cache;

/**
 * @brief Allocates and initializes a new cache.
 * @param config The configuration parameters for the cache.
 * @return A pointer to the newly created cache, or NULL on failure.
 */
Cache* cache_create(CacheConfig config);

/**
 * @brief Simulates a memory access to the cache.
 * @param cache The cache to access.
 * @param address The 64-bit memory address being accessed.
 */
void cache_access(Cache* cache, uint64_t address);

/**
 * @brief Frees all memory associated with the cache.
 * @param cache The cache to destroy.
 */
void cache_destroy(Cache* cache);

/**
 * @brief Prints the final cache statistics.
 * @param cache The cache.
 */
void print_stats(Cache* cache);

#endif  // CACHE_H