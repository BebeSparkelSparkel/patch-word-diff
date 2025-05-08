OBJS = cli.o error.o logging.o mfile.o parse.o parseState.o patchControl.o stateMachine.o tmpfile.o transfer.o main.o 

TARGET = patchw

: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.d: %.c
	$(CC) -M $(CPPFLAGS) $< | sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@

DEPS = $(OBJS:.o=.d)
-include $(DEPS)

clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)
