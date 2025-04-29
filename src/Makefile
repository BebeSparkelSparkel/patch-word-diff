OBJS = error.o logging.o main.o mfile.o parseState.o patchControl.o tmpfile.o transfer.o

patchw: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.d: %.c
	$(CC) -M $(CPPFLAGS) $< | sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@

DEPS = $(OBJS:.o=.d)
-include $(DEPS)

