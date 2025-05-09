PATCHW ?= ../src/patchw
PATCHW_ABS := $(abspath $(PATCHW))

TEST_DIRS := $(wildcard [0-9]*_*)

YELLOW = \033[0;33m
RESET = \033[0m

.PHONY: test clean $(TEST_DIRS)

test:
	@echo "$(YELLOW)Running patchw tests...$(RESET)"
	@echo "----------------------------"
	@$(MAKE) $(TEST_DIRS)
	@echo "----------------------------"
	@echo "Test summary: All tests completed"
	@echo "----------------------------"

$(TEST_DIRS):
	@./run_test.sh $@ "$(PATCHW_ABS)" "$(PATCHW_ARGS)"

clean:
	@echo "Cleaning test files..."
	@find . '(' -name "inplace*" -or -name "*.core" ')' -and -type f -delete

