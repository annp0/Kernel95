AS:=as
LD:=ld -m elf_x86_64 -T link.ld

LDFLAG:=-s --oformat binary

all: image disk_image

image: kernel.img

disk_image: hd10meg.img

kernel/system:
	cd kernel; make system; cd ..

kernel.img: tools/build bootsect setup kernel/system
	tools/build bootsect setup kernel/system > $@

tools/build: tools/build.c
	gcc -o $@ $<

bootsect: bootsect.o
	$(LD) $(LDFLAG) -o $@ $<

bootsect.o: bootsect.S
	$(AS) -o $@ $<

setup: setup.o
	$(LD) $(LDFLAG) -e _start_setup -o $@ $<

setup.o: setup.S
	$(AS) -o $@ $<

hd10meg.img:
	echo -e "1\nhd\nflat\n512\n10\nhd10meg.img\n" | bximage

clean:
	rm -rf *.o bootsect setup kernel.img tools/build bochslog.txt
	cd kernel; make clean; cd ..
	cd lib; make clean; cd ..

run: all
	bochs