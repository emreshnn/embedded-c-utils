CC       := gcc
BUILD    := build
UNITY    := $(BUILD)/unity/src
EXE      := $(if $(filter Windows_NT,$(OS)),.exe,)

CFLAGS   := -std=c99 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -Iinclude
SRCS     := $(wildcard src/*.c)
TESTS    := $(patsubst tests/%.c,$(BUILD)/%$(EXE),$(wildcard tests/test_*.c))

ARM_CC     := arm-none-eabi-gcc
ARM_FLAGS  := -std=c99 -mcpu=cortex-m4 -mthumb -Os -ffunction-sections \
              -Wall -Wextra -Wconversion -Werror -Iinclude

.PHONY: test check arm clean

# Build and run every unit test; stop at the first failing suite.
test: $(TESTS)
	@for t in $(TESTS); do ./$$t || exit 1; done

$(BUILD)/%$(EXE): tests/%.c $(SRCS) $(BUILD)/unity.o
	$(CC) $(CFLAGS) -I$(UNITY) $< $(SRCS) $(BUILD)/unity.o -o $@

# Unity is fetched on first build instead of being copied into the repo.
$(BUILD)/unity.o:
	git -c advice.detachedHead=false clone --quiet --depth 1 --branch v2.6.0 https://github.com/ThrowTheSwitch/Unity.git $(BUILD)/unity
	$(CC) -std=c99 -c $(UNITY)/unity.c -o $@

# Static analysis, including the MISRA C:2012 addon.
# Deviations (advisory rules, see README): 2.5 and 8.7 flag public API that
# the library itself does not use, which is expected for a library.
CPPCHECK ?= cppcheck
check:
	$(CPPCHECK) --enable=warning,style,performance,portability --std=c99 \
	         --addon=misra --suppress=misra-c2012-2.5 --suppress=misra-c2012-8.7 \
	         --error-exitcode=1 --inline-suppr -q -Iinclude src

# Prove the library builds for a Cortex-M4 target and report flash/RAM usage.
arm:
	@mkdir -p $(BUILD)/arm
	@for f in $(SRCS); do $(ARM_CC) $(ARM_FLAGS) -c $$f -o $(BUILD)/arm/$$(basename $$f .c).o || exit 1; done
	arm-none-eabi-size -t $(BUILD)/arm/*.o

clean:
	rm -rf $(BUILD)
