#include "unity.h"
#include "crc.h"

/* Standard check input used by every CRC catalogue. */
static const uint8_t check[] = "123456789";
#define CHECK_LEN 9U

void setUp(void) {}
void tearDown(void) {}

static void test_crc16_check_value(void)
{
    TEST_ASSERT_EQUAL_HEX16(0x29B1U, crc16_ccitt(check, CHECK_LEN, CRC16_INIT));
}

static void test_crc32_check_value(void)
{
    TEST_ASSERT_EQUAL_HEX32(0xCBF43926U, crc32(check, CHECK_LEN, CRC32_INIT));
}

static void test_chunked_equals_single_pass(void)
{
    uint16_t c16 = crc16_ccitt(check, 4U, CRC16_INIT);
    uint32_t c32 = crc32(check, 4U, CRC32_INIT);

    c16 = crc16_ccitt(&check[4], CHECK_LEN - 4U, c16);
    c32 = crc32(&check[4], CHECK_LEN - 4U, c32);
    TEST_ASSERT_EQUAL_HEX16(0x29B1U, c16);
    TEST_ASSERT_EQUAL_HEX32(0xCBF43926U, c32);
}

static void test_detects_single_bit_flip(void)
{
    uint8_t data[CHECK_LEN];

    for (size_t i = 0U; i < CHECK_LEN; i++) {
        data[i] = check[i];
    }
    data[3] ^= 0x01U;
    TEST_ASSERT_NOT_EQUAL(0x29B1U, crc16_ccitt(data, CHECK_LEN, CRC16_INIT));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_crc16_check_value);
    RUN_TEST(test_crc32_check_value);
    RUN_TEST(test_chunked_equals_single_pass);
    RUN_TEST(test_detects_single_bit_flip);
    return UNITY_END();
}
