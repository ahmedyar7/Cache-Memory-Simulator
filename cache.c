#include "cache.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

/**
 * @brief Finds a victim line in a given set based on the replacement policy.
 * @param cache The cache.
 * @param set_start A pointer to the first line of the set.
 * @return A pointer to the victim cache line to be replaced.
 */
static CacheLine* find_victim(Cache* cache, CacheLine* set_start) {
    CacheLine* victim = &set_start[0];

    if (strcmp(cache->config.policy, "LRU") == 0) {
        uint32_t min_time = UINT_MAX;
        for (uint32_t i = 0; i < cache->config.associativity; i++) {
            if (set_start[i].last_access_time < min_time) {
                min_time = set_start[i].last_access_time;
                victim = &set_start[i];
            }
        }
    } else if (strcmp(cache->config.policy, "FIFO") == 0) {
        uint32_t min_time = UINT_MAX;
        for (uint32_t i = 0; i < cache->config.associativity; i++) {
            if (set_start[i].arrival_time < min_time) {
                min_time = set_start[i].arrival_time;
                victim = &set_start[i];
            }
        }
    }
    // Default to first line if policy is unknown (shouldn't happen)
    return victim;
}

Cache* cache_create(CacheConfig config) {
    Cache* cache = (Cache*)malloc(sizeof(Cache));
    if (!cache) return NULL;

    cache->config = config;
    cache->stats = (CacheStats){0, 0, 0};
    cache->current_time = 0;

    uint32_t num_lines = config.cache_size / config.block_size;

    // Handle fully associative case
    if (config.associativity == 0 || config.associativity > num_lines) {
        cache->config.associativity = num_lines;  // All lines in one set
    }

    cache->num_sets = num_lines / cache->config.associativity;
    cache->block_bits = log2_int(config.block_size);
    cache->set_bits = log2_int(cache->num_sets);

    // Use calloc to initialize all lines to zero (valid=false)
    cache->lines = (CacheLine*)calloc(num_lines, sizeof(CacheLine));
    if (!cache->lines) {
        free(cache);
        return NULL;
    }

    return cache;
}

void cache_access(Cache* cache, uint64_t address) {
    cache->stats.accesses++;
    cache->current_time++;

    uint64_t tag = get_tag(address, cache->set_bits, cache->block_bits);
    uint64_t index = get_index(address, cache->set_bits, cache->block_bits);

    // Get a pointer to the start of the correct set
    CacheLine* set_start = &cache->lines[index * cache->config.associativity];

    // 1. Check for Hit
    for (uint32_t i = 0; i < cache->config.associativity; i++) {
        CacheLine* line = &set_start[i];
        if (line->valid && line->tag == tag) {
            cache->stats.hits++;
            if (strcmp(cache->config.policy, "LRU") == 0) {
                line->last_access_time = cache->current_time;
            }
            return;  // Hit!
        }
    }

    // 2. Miss
    cache->stats.misses++;

    // 3. Find empty line
    for (uint32_t i = 0; i < cache->config.associativity; i++) {
        CacheLine* line = &set_start[i];
        if (!line->valid) {
            line->valid = true;
            line->tag = tag;
            line->last_access_time = cache->current_time;
            line->arrival_time = cache->current_time;
            return;  // Miss, placed in empty line
        }
    }

    // 4. Eviction (no empty lines found)
    CacheLine* victim = find_victim(cache, set_start);
    victim->valid = true;
    victim->tag = tag;
    victim->last_access_time = cache->current_time;
    victim->arrival_time = cache->current_time;
}

void cache_destroy(Cache* cache) {
    if (!cache) return;
    free(cache->lines);
    free(cache);
}

void print_stats(Cache* cache) {
    printf("--- Cache Simulation Stats ---\n");
    printf("Total Accesses: %u\n", cache->stats.accesses);
    printf("Total Hits:     %u\n", cache->stats.hits);
    printf("Total Misses:   %u\n", cache->stats.misses);

    double hit_rate = (double)cache->stats.hits / cache->stats.accesses;
    printf("Hit Rate:       %.2f%%\n", hit_rate * 100.0);
    printf("Miss Rate:      %.2f%%\n", (1.0 - hit_rate) * 100.0);
    printf("------------------------------\n");
}