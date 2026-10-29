# embedded-c-utils

[![CI](https://github.com/emreshnn/embedded-c-utils/actions/workflows/ci.yml/badge.svg)](https://github.com/emreshnn/embedded-c-utils/actions/workflows/ci.yml)

Small, dependency-free C99 building blocks for bare-metal and RTOS firmware.
No dynamic memory, no recursion, MISRA C:2012 checked, unit tested on host and
cross-compiled for ARM Cortex-M4 on every push.

| Module | What it does | Typical use |
|---|---|---|
| [`ringbuf`](include/ringbuf.h) | Lock-free SPSC byte FIFO | UART RX interrupt → main loop |
| [`crc`](include/crc.h) | CRC-16/CCITT-FALSE, CRC-32 (table-free, chainable) | Packet / firmware image integrity |
| [`mempool`](include/mempool.h) | Fixed-size block allocator, O(1) alloc | Message buffers without `malloc` |
| [`fsm`](include/fsm.h) | Table-driven state machine | System modes, protocol handlers |

### Footprint

Cortex-M4, Thumb, `-Os`, arm-none-eabi-gcc 14.2 (from the [CI](.github/workflows/ci.yml) log):

| Module | Flash (bytes) | RAM (bytes) |
|---|---:|---:|
| crc | 100 | 0 |
| fsm | 72 | 0 |
| mempool | 148 | 0 |
| ringbuf | 112 | 0 |
| **Total** | **432** | **0** |

## Design decisions

- **No heap.** All memory is supplied by the caller, so usage is known at link time.
- **Ring buffer without locks.** Only the ISR writes `head`, only the main loop writes
  `tail`. Indices run freely and are masked on access (capacity is a power of two),
  so `head - tail` is the fill level even after integer wrap-around.
- **Table-free CRC.** Trades a few CPU cycles for not storing 512 B / 1 KB lookup tables in flash.
  Both functions accept the previous CRC, so data can be checksummed as it streams in.
- **Defensive `mempool_free`.** Pointers are validated by equality against each block
  instead of `<`/`>` comparisons, which are undefined behaviour for unrelated objects.
  A stray pointer is rejected rather than corrupting the free list.
- **FSM as data.** Transitions live in a `const` table (in flash). The table can be
  reviewed line by line against requirements, and events invalid in the current state
  are rejected explicitly.

## Example

```c
static uint8_t rx_storage[64];
static ringbuf_t rx;

void USART1_IRQHandler(void)            /* producer */
{
    (void)ringbuf_put(&rx, (uint8_t)USART1->DR);
}

int main(void)                          /* consumer */
{
    uint8_t byte;

    (void)ringbuf_init(&rx, rx_storage, sizeof rx_storage);
    for (;;) {
        while (ringbuf_get(&rx, &byte)) {
            parser_feed(byte);
        }
    }
}
```

The [FSM tests](tests/test_fsm.c) model a typical power-on sequence:
`BIT → STANDBY → OPERATIONAL`, with `FAULT` reachable from Built-In Test or operation.

## Build

Requires `gcc`, `make` and `git` (Unity is fetched automatically on first build).

```sh
make test     # build and run all unit tests (Unity)
make check    # cppcheck + MISRA C:2012 addon
make arm      # cross-compile for Cortex-M4 (arm-none-eabi-gcc) and print sizes
```

## Quality gates

Every push runs the following in [GitHub Actions](.github/workflows/ci.yml):

1. Compile with `-std=c99 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror`
2. 16 unit tests covering boundaries: full/empty, index overflow, foreign pointers,
   invalid events, and standard CRC check values (`"123456789"`)
3. cppcheck static analysis with the MISRA C:2012 addon
4. Cross-compile for `cortex-m4` / Thumb with `-Os`

### MISRA deviations

| Rule | Category | Reason |
|---|---|---|
| 2.5 | Advisory | Public macros (`CRC16_INIT` …) are used by clients, not by the library itself |
| 8.7 | Advisory | Public API functions must have external linkage even if unused inside the library |

## License

MIT
