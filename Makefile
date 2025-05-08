# Root Makefile for patchw

# Directories
SRC_DIR := src
TEST_DIR := tests

# Target binary
TARGET  := $(SRC_DIR)/patchw

.PHONY: all clean test

# Default target
all: $(TARGET)

# Build the target - calls the src/Makefile recursively
$(TARGET):
	$(MAKE) -C $(SRC_DIR)

# Clean build artifacts
clean:
	$(MAKE) -C $(SRC_DIR) clean
	$(MAKE) -C $(TEST_DIR) clean

# Run tests
test: $(TARGET)
	$(MAKE) -C $(TEST_DIR) test PATCHW=$(CURDIR)/$(TARGET)
