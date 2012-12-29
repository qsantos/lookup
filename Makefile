CFLAGS  = -Wall -Wextra -Werror -pedantic -std=c99 -O3 -D_XOPEN_SOURCE=600 -g
LDFLAGS = -O3
TARGET  = ltgen
OBJ     = $(SRC:.c=.o)

all: $(TARGET)

lt%: lt%.o md5.o
	gcc $(LDFLAGS) $^ -o $@

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o

destroy: clean
	rm -f $(TARGET)

rebuild: destroy all
