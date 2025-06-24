GCC := gcc
USE_DEBUG :=
CCFLAG := -I../include -nostdinc -ffreestanding -Wall -fomit-frame-pointer -fno-pic -fno-stack-protector -c -m32 $(USE_DEBUG)
LDFLAG := -T ../link.ld -s --oformat binary -m elf_i386
INCDIR := ../include
OBJS   := head.o main.o sched.o chr_drv/chr_drv.a

system: $(OBJS)
	$(LD) $(LDFLAG) -e startup_32 -o $@ $^

head.o : head.S
	$(GCC) -m32 -traditional -c -o $@ $<

main.o : main.c
	$(GCC) $(CCFLAG) -o $@ $<

sched.o : sched.c
	$(GCC) $(CCFLAG) -o $@ $<

chr_drv/chr_drv.a: chr_drv/*.c
	cd chr_drv; make chr_drv.a; cd ..

clean :
	rm -f *.o
	rm -f system
	cd chr_drv; make clean; cd ..