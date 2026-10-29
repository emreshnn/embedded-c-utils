#include "unity.h"
#include "mempool.h"

#define BLOCK_SIZE  8U
#define BLOCK_COUNT 3U

static mempool_t mp;
static uint8_t storage[BLOCK_SIZE * BLOCK_COUNT];

void setUp(void) { (void)mempool_init(&mp, storage, BLOCK_SIZE, BLOCK_COUNT); }
void tearDown(void) {}

static void test_init_rejects_bad_args(void)
{
    TEST_ASSERT_FALSE(mempool_init(&mp, NULL, BLOCK_SIZE, BLOCK_COUNT));
    TEST_ASSERT_FALSE(mempool_init(&mp, storage, 0U, BLOCK_COUNT));
    TEST_ASSERT_FALSE(mempool_init(&mp, storage, BLOCK_SIZE, 0U));
    TEST_ASSERT_FALSE(mempool_init(&mp, storage, 1U, 255U));
}

static void test_alloc_until_exhausted(void)
{
    void *a = mempool_alloc(&mp);
    void *b = mempool_alloc(&mp);
    void *c = mempool_alloc(&mp);

    TEST_ASSERT_NOT_NULL(a);
    TEST_ASSERT_NOT_NULL(b);
    TEST_ASSERT_NOT_NULL(c);
    TEST_ASSERT_TRUE((a != b) && (b != c) && (a != c));
    TEST_ASSERT_NULL(mempool_alloc(&mp));
    TEST_ASSERT_EQUAL_UINT8(0U, mp.free_count);
}

static void test_free_makes_block_reusable(void)
{
    void *a = mempool_alloc(&mp);

    (void)mempool_alloc(&mp);
    (void)mempool_alloc(&mp);
    TEST_ASSERT_TRUE(mempool_free(&mp, a));
    TEST_ASSERT_EQUAL_PTR(a, mempool_alloc(&mp));
}

static void test_free_rejects_foreign_pointers(void)
{
    uint8_t other = 0U;

    TEST_ASSERT_FALSE(mempool_free(&mp, NULL));
    TEST_ASSERT_FALSE(mempool_free(&mp, &other));
    TEST_ASSERT_FALSE(mempool_free(&mp, &storage[1]));  /* mid-block */
    TEST_ASSERT_EQUAL_UINT8(BLOCK_COUNT, mp.free_count);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_init_rejects_bad_args);
    RUN_TEST(test_alloc_until_exhausted);
    RUN_TEST(test_free_makes_block_reusable);
    RUN_TEST(test_free_rejects_foreign_pointers);
    return UNITY_END();
}
