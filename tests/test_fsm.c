#include "unity.h"
#include "fsm.h"

/* Typical system mode logic: power-on Built-In Test, then standby/operate. */
enum { ST_BIT, ST_STANDBY, ST_OPERATIONAL, ST_FAULT };
enum { EV_BIT_PASS, EV_BIT_FAIL, EV_START, EV_STOP, EV_ERROR, EV_RESET };

static int fault_count;
static void on_fault(void *ctx) { (void)ctx; fault_count++; }

static const fsm_transition_t table[] = {
    { ST_BIT,         EV_BIT_PASS, ST_STANDBY,     NULL     },
    { ST_BIT,         EV_BIT_FAIL, ST_FAULT,       on_fault },
    { ST_STANDBY,     EV_START,    ST_OPERATIONAL, NULL     },
    { ST_OPERATIONAL, EV_STOP,     ST_STANDBY,     NULL     },
    { ST_OPERATIONAL, EV_ERROR,    ST_FAULT,       on_fault },
    { ST_FAULT,       EV_RESET,    ST_BIT,         NULL     },
};

static fsm_t fsm;

void setUp(void)
{
    fault_count = 0;
    fsm_init(&fsm, table, sizeof table / sizeof table[0], ST_BIT, NULL);
}
void tearDown(void) {}

static void test_normal_startup(void)
{
    TEST_ASSERT_TRUE(fsm_dispatch(&fsm, EV_BIT_PASS));
    TEST_ASSERT_TRUE(fsm_dispatch(&fsm, EV_START));
    TEST_ASSERT_EQUAL_UINT8(ST_OPERATIONAL, fsm.state);
}

static void test_invalid_event_is_ignored(void)
{
    TEST_ASSERT_FALSE(fsm_dispatch(&fsm, EV_START));  /* not allowed during BIT */
    TEST_ASSERT_EQUAL_UINT8(ST_BIT, fsm.state);
}

static void test_fault_runs_action_and_recovers(void)
{
    TEST_ASSERT_TRUE(fsm_dispatch(&fsm, EV_BIT_FAIL));
    TEST_ASSERT_EQUAL_UINT8(ST_FAULT, fsm.state);
    TEST_ASSERT_EQUAL_INT(1, fault_count);
    TEST_ASSERT_TRUE(fsm_dispatch(&fsm, EV_RESET));
    TEST_ASSERT_EQUAL_UINT8(ST_BIT, fsm.state);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_normal_startup);
    RUN_TEST(test_invalid_event_is_ignored);
    RUN_TEST(test_fault_runs_action_and_recovers);
    return UNITY_END();
}
