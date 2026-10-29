#include "crc.h"

uint16_t crc16_ccitt(const uint8_t *data, size_t len, uint16_t crc)
{
    uint16_t c = crc;

    for (size_t i = 0U; i < len; i++) {
        c ^= (uint16_t)((uint16_t)data[i] << 8U);
        for (uint8_t bit = 0U; bit < 8U; bit++) {
            if ((c & 0x8000U) != 0U) {
                c = (uint16_t)((uint16_t)(c << 1U) ^ 0x1021U);
            } else {
                c = (uint16_t)(c << 1U);
            }
        }
    }
    return c;
}

uint32_t crc32(const uint8_t *data, size_t len, uint32_t crc)
{
    uint32_t c = ~crc;

    for (size_t i = 0U; i < len; i++) {
        c ^= (uint32_t)data[i];
        for (uint8_t bit = 0U; bit < 8U; bit++) {
            if ((c & 1U) != 0U) {
                c = (c >> 1U) ^ 0xEDB88320U;
            } else {
                c >>= 1U;
            }
        }
    }
    return ~c;
}
