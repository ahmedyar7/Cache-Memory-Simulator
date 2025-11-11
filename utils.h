#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

/**
 * @brief Calculates integer log base 2.
 * @param n The number.
 * @return log2(n).
 */
uint32_t log2_int(uint32_t n);

/**
 * @brief Extracts the tag from a 64-bit address.
 */
uint64_t get_tag(uint64_t address, uint32_t set_bits, uint32_t block_bits);

/**
 * @brief Extracts the set index from a 64-bit address.
 */
uint64_t get_index(uint64_t address, uint32_t set_bits, uint32_t block_bits);

#endif  // UTILS_H