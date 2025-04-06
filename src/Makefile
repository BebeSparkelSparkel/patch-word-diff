# ./src/Makefile

.PRECIOUS: %.d

CFLAGS = -Wall
ifdef DEBUG
	CFLAGS += -Werror -g
endif
ifdef C_ERROR_LIMIT
	CFLAGS += -ferror-limit=$(C_ERROR_LIMIT)
endif
YFLAGS = -d

TARGET = diff_parser.out
SOURCES := $(shell git ls-files)

all: $(TARGET)

$(TARGET): main.o diff_tree.o parser.o lexer.o string_builder.o json_common.o
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
		> parser.c
	rm y.tab.c
	mv y.tab.h parser.h

%.d: %.c
	$(CC) -MM $< -MT $*.o \
		| sed '1s/:/ $@:/' \
		> $@

%.o: %.c
%.o: %.c %.d
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) *.o *.d parser.? parser_element.bnf parser_line_map.awk

-include $(wildcard *.d)

.PHONY: all clean

