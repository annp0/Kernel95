AR := ar
LD := ld
GCC := gcc

CCFLAG := -m32 -I../../include -nostdinc -Wall -fomit-frame-pointer -c

OBJS := tty_io.o console.o

tty_io.o: tty_io.c
	$(GCC) $(CCFLAG) -o $@ $<

console.o: console.c
	$(GCC) $(CCFLAG) -o $@ $<

chr_drv.a: $(OBJS)
	$(AR) rcs $@ $^

clean:
	rm -f *.o
	rm -f chr_drv.a
