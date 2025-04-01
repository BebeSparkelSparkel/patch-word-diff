.SUFFIXES:

.PRECIOUS: %.d

CFLAGS = -Wall -Werror
ifeq ($(CC),clang)
  CFLAGS += -fmacro-backtrace-limit=0 -fdiagnostics-show-category=name
endif
YFLAGS = -d

TARGET = diff_parser
SOURCES := $(shell git ls-files)

all: $(TARGET)

$(TARGET): diff_tree.o parser.o lexer.o
	$(CC) $(CFLAGS) -o $@ $^

parser_element.bnf: parser_element_bnf.cpp element.h
	$(CC) -E $< | grep -v -e '^#' -e '^$$' > $@

PARSER_COMPONENTS = parser.dec parser.bnf parser_element.bnf parser.cnd
parser.y parser_line_map.awk: $(PARSER_COMPONENTS) line_map.awk
	wc -l $(PARSER_COMPONENTS) | awk -f line_map.awk > parser_line_map.awk
	cat $(PARSER_COMPONENTS) > parser.y

parser.c parser.h: parser.y parser_line_map.awk
	$(YACC) $(YFLAGS) $<
	cat y.tab.c \
		| sed 's/y\.tab\.\([ch]\)/parser.\1/g' \
		| awk -f parser_line_map.awk   \
		>> parser.c
	rm y.tab.c
	mv y.tab.h parser.h

%.d: %.c
	echo -n "$@ " > $@
	$(CC) -MM $< -MT $*.o >> $@

%.o: %.c %.d
	$(CC) $(CFLAGS) -c -o $@ $<

-include $(wildcard *.d)

clean:
	rm -f $(TARGET) *.o *.d parser.?

.PHONY: all clean

