/**
 * @file  crc.h
 * @brief Table-free CRC routines (small flash footprint).
 *
 * Both functions can be chained to checksum data that arrives in chunks:
 *   crc = crc16_ccitt(part1, n1, CRC16_INIT);
 *   crc = crc16_ccitt(part2, n2, crc);
 */
#ifndef CRC_H
#define CRC_H

#include <stddef.h>
#include <stdint.h>

#define CRC16_INIT 0xFFFFU  /**< Start value for crc16_ccitt */
#define CRC32_INIT 0U       /**< Start value for crc32 */

/** CRC-16/CCITT-FALSE: poly 0x1021, init 0xFFFF, no reflection, no final XOR. */
uint16_t crc16_ccitt(const uint8_t *data, size_t len, uint16_t crc);

/** CRC-32 (IEEE 802.3, same as zlib/Ethernet): reflected poly 0xEDB88320. */
uint32_t crc32(const uint8_t *data, size_t len, uint32_t crc);

#endif /* CRC_H */
