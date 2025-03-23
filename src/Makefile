CFLAGS = -Wall -Werror
YFLAGS = -d

ELEMENT_MAPPINGS = Element:diff_tree.h Diff:diff_tree.h Hunk:diff_tree.h Patch:diff_tree.h
SLL_TYPES = $(foreach mapping,$(ELEMENT_MAPPINGS),$(firstword $(subst :, ,$(mapping))))

CORE_OBJS = diff_tree.o parser.o lexer.o
SLL_OBJS = $(foreach type,$(SLL_TYPES),singly_linked_list_$(type).o)
OBJS = $(CORE_OBJS) $(SLL_OBJS)

TARGET = diff_parser

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

diff_tree.h: string_builder.h element.h json.h

diff_tree.o: diff_tree.h json.c

element_include = $(lastword $(subst :, ,$(filter $(1):%,$(ELEMENT_MAPPINGS))))
define element_template
$(1)_$(2).o: $(1).c $(1).h $(call element_include,$(2))
	$$(CC) $$(CFLAGS) -DSLL_TYPE=$(2) \
	$$(if $$(call element_include,$(2)),-DSLL_INCLUDE=\"$$(call element_include,$(2))\") \
	-c $$< -o $$@
endef
$(foreach type,$(SLL_TYPES),$(eval $(call element_template,singly_linked_list,$(type))))

parser_pp.y: parser.y element.h
	$(CC) -E -x c -o $@ $<

parser.c parser.h: parser_pp.y
	$(YACC) $(YFLAGS) $<
	mv y.tab.c parser.c
	mv y.tab.h parser.h

parser.o: parser.c parser.h diff_tree.h

lexer.o: diff_tree.h parser.h

clean:
	rm -f $(TARGET) $(OBJS) parser_pp.y parser.c parser.h lexer.c

