CC=gcc
CFLAGS=-Wall -pedantic -g -DUNIX_HOST -DVER=\"0.0.1\"
LIBS=-lm

TARGET	= hawthorn
SRCS	= hawthorn.c table.c lex.c parse.c expression.c heap.c type.c \
	variable.c clibrary.c platform.c include.c debug.c \
	platform/platform_unix.c platform/library_unix.c \
	cstdlib/stdio.c cstdlib/math.c cstdlib/string.c cstdlib/stdlib.c \
	cstdlib/time.c cstdlib/errno.c cstdlib/ctype.c cstdlib/stdbool.c \
	cstdlib/unistd.c
OBJS	:= $(SRCS:%.c=%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

test:	all
	(cd tests; make test)

clean:
	rm -f $(TARGET) $(OBJS) *~

count:
	@echo "Core:"
	@cat hawthorn.h interpreter.h hawthorn.c table.c lex.c parse.c expression.c platform.c heap.c type.c variable.c include.c debug.c | grep -v '^[ 	]*/\*' | grep -v '^[ 	]*$$' | wc
	@echo ""
	@echo "Everything:"
	@cat $(SRCS) *.h */*.h | wc

.PHONY: clibrary.c

hawthorn.o: hawthorn.c hawthorn.h
table.o: table.c interpreter.h platform.h
lex.o: lex.c interpreter.h platform.h
parse.o: parse.c hawthorn.h interpreter.h platform.h
expression.o: expression.c interpreter.h platform.h
heap.o: heap.c interpreter.h platform.h
type.o: type.c interpreter.h platform.h
variable.o: variable.c interpreter.h platform.h
clibrary.o: clibrary.c hawthorn.h interpreter.h platform.h
platform.o: platform.c hawthorn.h interpreter.h platform.h
include.o: include.c hawthorn.h interpreter.h platform.h
debug.o: debug.c interpreter.h platform.h
platform/platform_unix.o: platform/platform_unix.c hawthorn.h interpreter.h platform.h
platform/library_unix.o: platform/library_unix.c interpreter.h platform.h
cstdlib/stdio.o: cstdlib/stdio.c interpreter.h platform.h
cstdlib/math.o: cstdlib/math.c interpreter.h platform.h
cstdlib/string.o: cstdlib/string.c interpreter.h platform.h
cstdlib/stdlib.o: cstdlib/stdlib.c interpreter.h platform.h
cstdlib/time.o: cstdlib/time.c interpreter.h platform.h
cstdlib/errno.o: cstdlib/errno.c interpreter.h platform.h
cstdlib/ctype.o: cstdlib/ctype.c interpreter.h platform.h
cstdlib/stdbool.o: cstdlib/stdbool.c interpreter.h platform.h
cstdlib/unistd.o: cstdlib/unistd.c interpreter.h platform.h
