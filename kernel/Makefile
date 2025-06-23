GCC := gcc
CCFLAG := -mcpu=i386 -I../include -nostdinc -Wall -fomit-frame-pointer -c
LDFLAG := -Ttext 0x0 -T ../link.ld -s --oformat binary
OBJS := head.o

system: $(OBJS)
	$(LD) $(LDFLAG) -e startup_32 -o $@ $^

head.o : head.S
	$(GCC) -c -o $@ $<

clean :
	rm *.o
	rm system