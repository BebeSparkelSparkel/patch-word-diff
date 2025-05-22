PATCHW ?= ../src/patchw
PATCHW_ABS := $(abspath $(PATCHW))

TEST_DIRS := $(wildcard [0-9]*_*)

YELLOW = \033[0;33m
RED = \033[0;31m
CYAN = \033[0;36m
RESET = \033[0m

.PHONY: test clean $(TEST_DIRS) pretty-print pending

test:
	@echo "$(YELLOW)Running patchw tests...$(RESET)"
	@echo "----------------------------"
	@$(MAKE) $(TEST_DIRS)
	@echo "----------------------------"
	@echo "Test summary: All tests completed"
	@echo "----------------------------"

%/patch.txt: %/source.txt %/expect.txt
	@cd $* && git diff --word-diff --no-index source.txt expect.txt | \
		sed -E 's/(source|expect)\.txt/inplace.txt/g' > $(notdir $@)
	@chmod a-w $@

$(TEST_DIRS): %: %/patch.txt
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

pending:
	@if [ -z "$(TEST)" ]; then \
		echo -e "$(RED)Error: Please specify a test directory with TEST=directory_name$(RESET)"; \
		echo "Example: make pending TEST=200_some-test REASON=\"Waiting for feature X\""; \
		exit 1; \
	fi; \
	if [ ! -d "$(TEST)" ]; then \
		echo -e "$(RED)Error: Test directory '$(TEST)' not found$(RESET)"; \
		exit 1; \
	fi; \
	if [ -n "$(REASON)" ]; then \
		echo "$(REASON)" > "$(TEST)/pending.txt"; \
		echo -e "$(CYAN)Marked $(TEST) as pending: $(REASON)$(RESET)"; \
	else \
		touch "$(TEST)/pending.txt"; \
		echo -e "$(CYAN)Marked $(TEST) as pending$(RESET)"; \
	fi
