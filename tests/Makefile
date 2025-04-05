# ./tests/Makefile
.PRECIOUS: %.d

# Same compiler flags as src plus additional flags for Check
CFLAGS = -Wall -Werror -I../src -I/usr/local/include
CHECK_LIBS = -lcheck
LDFLAGS=-L/usr/local/lib

TEST_TARGETS = test_string_builder
SOURCES = $(wildcard *.c)

all: $(TEST_TARGETS)

# Rule for test_string_builder
test_string_builder: test_string_builder.o ../src/string_builder.o ../src/json_common.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(CHECK_LIBS) 

# Rule for building object files
%.o: %.c %.d
	$(CC) $(CFLAGS) -c -o $@ $<

# Generate dependencies
%.d: %.c
	@echo "Creating dependency file $@..."
	@mkdir -p $(dir $@)
	@echo -n "$@ " > $@
	@$(CC) -MM -I../src $< -MT $(patsubst %.d,%.o,$@) >> $@

-include $(wildcard *.d)

# Run all tests
run: $(TEST_TARGETS)
	@echo "Running all tests..."
	@for test in $(TEST_TARGETS); do ./$$test; done

# Clean
clean:
	rm -f $(TEST_TARGETS) *.o *.d

.PHONY: all clean run
