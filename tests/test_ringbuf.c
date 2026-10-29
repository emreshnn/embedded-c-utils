#include "unity.h"
#include "ringbuf.h"

static ringbuf_t rb;
static uint8_t storage[4];

void setUp(void) { (void)ringbuf_init(&rb, storage, sizeof storage); }
void tearDown(void) {}

static void test_init_rejects_non_power_of_two(void)
{
    TEST_ASSERT_FALSE(ringbuf_init(&rb, storage, 3U));
    TEST_ASSERT_FALSE(ringbuf_init(&rb, storage, 0U));
    TEST_ASSERT_FALSE(ringbuf_init(&rb, NULL, 4U));
}

static void test_fifo_order(void)
{
    uint8_t b = 0U;

    TEST_ASSERT_TRUE(ringbuf_put(&rb, 1U));
    TEST_ASSERT_TRUE(ringbuf_put(&rb, 2U));
    TEST_ASSERT_TRUE(ringbuf_get(&rb, &b));
    TEST_ASSERT_EQUAL_UINT8(1U, b);
    TEST_ASSERT_TRUE(ringbuf_get(&rb, &b));
    TEST_ASSERT_EQUAL_UINT8(2U, b);
}

static void test_full_and_empty(void)
{
    uint8_t b = 0U;

    TEST_ASSERT_FALSE(ringbuf_get(&rb, &b));
    for (uint8_t i = 0U; i < 4U; i++) {
        TEST_ASSERT_TRUE(ringbuf_put(&rb, i));
    }
    TEST_ASSERT_FALSE(ringbuf_put(&rb, 99U));
    TEST_ASSERT_EQUAL_size_t(4U, ringbuf_count(&rb));
}

static void test_wraps_around(void)
{
    uint8_t b = 0U;

    for (uint8_t i = 0U; i < 10U; i++) {
        TEST_ASSERT_TRUE(ringbuf_put(&rb, i));
        TEST_ASSERT_TRUE(ringbuf_get(&rb, &b));
        TEST_ASSERT_EQUAL_UINT8(i, b);
    }
    TEST_ASSERT_EQUAL_size_t(0U, ringbuf_count(&rb));
}

static void test_index_overflow(void)
{
    uint8_t b = 0U;

    /* Simulate indices that have been running for a very long time. */
    rb.head = SIZE_MAX;
    rb.tail = SIZE_MAX;
    TEST_ASSERT_TRUE(ringbuf_put(&rb, 0xAAU));
    TEST_ASSERT_TRUE(ringbuf_put(&rb, 0xBBU));
    TEST_ASSERT_EQUAL_size_t(2U, ringbuf_count(&rb));
    TEST_ASSERT_TRUE(ringbuf_get(&rb, &b));
    TEST_ASSERT_EQUAL_UINT8(0xAAU, b);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_init_rejects_non_power_of_two);
    RUN_TEST(test_fifo_order);
    RUN_TEST(test_full_and_empty);
    RUN_TEST(test_wraps_around);
    RUN_TEST(test_index_overflow);
    return UNITY_END();
}
