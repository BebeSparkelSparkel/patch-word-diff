# Root Makefile for patchw

TARGET := src/patchw

.PHONY: all clean test

all: $(TARGET)

$(TARGET):
	$(MAKE) -C $(SRC_DIR)

clean:
	$(MAKE) -C src clean
	$(MAKE) -C tests clean

test: $(TARGET)
	$(MAKE) -C tests $@ PATCHW=$(CURDIR)/$(TARGET)
