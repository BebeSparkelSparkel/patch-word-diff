# Makefile for patchw tests

# Default path to patchw binary, will be overridden by root Makefile
PATCHW ?= ../src/patchw

# Find all test directories
TEST_DIRS := $(wildcard [0-9]*_*)

# Variables for test summary
TOTAL_TESTS = 0
PASSED_TESTS = 0
FAILED_TESTS = 0

# Colors for output
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
RESET = \033[0m

.PHONY: test clean $(TEST_DIRS)

# Run all tests
test:
	@echo "$(YELLOW)Running patchw tests...$(RESET)"
	@echo "----------------------------"
	@$(MAKE) $(TEST_DIRS)
	@echo "----------------------------"
	@echo "Test summary: All tests completed"
	@echo "----------------------------"

# Run individual test
$(TEST_DIRS):
	@echo "Running test: $(YELLOW)$@$(RESET)"
	@cp $@/source.txt $@/inplace.txt
	@cd $@; $(PATCHW) ./patch.txt || (echo "$(RED)Failed$(RESET): patchw returned error code $?"; exit 1)
	@if diff -q $@/inplace.txt $@/result.txt > /dev/null; then \
		echo "$(GREEN)Passed$(RESET)"; \
	else \
		echo "$(RED)Failed$(RESET): Output doesn't match expected result"; \
		echo "Expected:"; \
		cat $@/result.txt; \
		echo "Got:"; \
		cat $@/inplace.txt; \
		exit 1; \
	fi

# Clean test artifacts
clean:
	@echo "Cleaning test files..."
	@find . -name "inplace.txt" -type f -delete
