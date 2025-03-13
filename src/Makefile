CFLAGS = -Wall
YFLAGS = -d

QUEUE_MAPPINGS = Element:queue_types.h Diff:diff_tree.h Hunk:diff_tree.h Patch:diff_tree.h
QUEUE_TYPES = $(foreach mapping,$(QUEUE_MAPPINGS),$(firstword $(subst :, ,$(mapping))))

CORE_OBJS = diff_tree.o parser.o lexer.o
QUEUE_OBJS = $(foreach type,$(QUEUE_TYPES),queue_$(type).o)
OBJS = $(CORE_OBJS) $(QUEUE_OBJS)

TARGET = diff_parser

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

diff_tree.h: queue.h

diff_tree.o: diff_tree.h

queue_include = $(lastword $(subst :, ,$(filter $(1):%,$(QUEUE_MAPPINGS))))
define queue_template
queue_$(1).o: queue.c queue.h $(call queue_include,$(1))
	$$(CC) $$(CFLAGS) -DQUEUE_TYPE=$(1) \
	$$(if $$(call queue_include,$(1)),-DQUEUE_INCLUDE=\"$$(call queue_include,$(1))\") \
	-c $$< -o $$@
endef
$(foreach type,$(QUEUE_TYPES),$(eval $(call queue_template,$(type))))

parser.c parser.h: parser.y
	$(YACC) $(YFLAGS) $<
	mv y.tab.c parser.c
	mv y.tab.h parser.h

parser.o: parser.c parser.h diff_tree.h

lexer.o: parser.h

clean:
	rm -f $(TARGET) $(OBJS) parser.c parser.h lexer.c

