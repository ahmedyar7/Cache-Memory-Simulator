#include "utils.h"

#include <math.h>  // For log2

uint32_t log2_int(uint32_t n) {
    if (n <= 1) return 0;  // log2(1) = 0
    return (uint32_t)log2(n);
}

uint64_t get_tag(uint64_t address, uint32_t set_bits, uint32_t block_bits) {
    // Shift off the set and block bits
    return address >> (set_bits + block_bits);
}

uint64_t get_index(uint64_t address, uint32_t set_bits, uint32_t block_bits) {
    if (set_bits == 0) {
        return 0;  // Fully associative, always index 0
    }
    // 1. Shift off the block bits
    uint64_t temp = address >> block_bits;
    // 2. Create a mask for the set bits
    uint64_t mask = (1UL << set_bits) - 1;
    // 3. AND with the mask to extract the index
    return temp & mask;
}