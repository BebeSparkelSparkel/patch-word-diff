PATCHW ?= ../src/patchw
PATCHW_ABS := $(abspath $(PATCHW))

TEST_DIRS := $(wildcard [0-9]*_*)

YELLOW = \033[0;33m
RED = \033[0;31m
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
	@./run_test.sh $@ "$(PATCHW_ABS)" "$(TEST_ARGS)"

clean:
	@echo "Cleaning test files..."
	@find . '(' -name "inplace*" -or -name "*.core" ')' -and -type f -delete

pretty-print:
	@if [ -z "$(TEST)" ]; then \
		echo -e "$(RED)Error: Please specify a test directory with TEST=directory_name$(RESET)"; \
		echo "Example: make pretty-print TEST=100_empty-source-addition-patch"; \
		exit 1; \
	fi
	@./pretty-print.sh "$(TEST)"
