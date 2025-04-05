# ./Makefile
.PHONY: all clean test src tests

all: src

src:
	$(MAKE) -C src

tests: src
	$(MAKE) -C tests

test: src tests
	$(MAKE) -C tests run

clean:
	$(MAKE) -C src clean
	$(MAKE) -C tests clean
