# Kernel98

Kernel98 is a small educational 32-bit operating system kernel, featuring:

* Copy-on-Write for `fork()`: When a process is forked, only the `task_struct` is duplicated initially. All other memory pages are shared between parent and child using Copy-on-Write (CoW). Physical pages are only copied when either process writes to them.
* Shared pages for `execv()`: When multiple processes execute the same binary, code pages are shared between them to reduce memory usage.
* Demand paging: Pages are only allocated from, or loaded into physical memory (depending on if the address is mapped to disk or not) when they are actually accessed. 
* Buffer cache for block devices: To speed up disk IO, all reads/writes are cached using reference-counted buffers. Each buffer has flags indicating whether it is up-to-date or dirty. Dirty buffers must be flushed to disk before reuse. Buffers are protected using mutexes to ensure safe concurrent access.

To build the kernel, run `make`. You can run it with Bochs.

## Design Notes

Here's a high-level overview of the boot and runtime design:

### Step 0: Bootstrapping and Entering 32-bit Protected Mode

When powered on, the CPU starts in 16-bit real mode, executing BIOS firmware.

* BIOS loads the bootloader (first 512 bytes of the disk) into memory at `0x7c00` and jumps to it.
* The bootloader quickly copies `setup.S` to a higher address (`0x90000`) to avoid being overwritten.
* It collects hardware information and loads the system image from disk to `0x10000`.
* A Global Descriptor Table (GDT) is set up with flat segments.
* A20 line is enabled and protected mode is entered.
* BIOS is no longer usable, so the system image is moved to `0x0`.
* Paging is enabled using a single Page Directory Table (PDT) and four Page Tables to create a 1:1 identity mapping covering 16 GB.
* Physical memory is divided into 4 KB pages with a reference count array that tracks usage.
* All external interrupts are temporarily disabled during this setup.

### Step 1: Console and Formatted Printing

* The VGA text buffer is mapped at `0xb8000`. Cursor position is controlled by writing to specific VGA I/O ports.
* A minimal `printf` implementation walks the stack according to the control string, and format it into a buffer, which is then written directly to VGA memory.

### Step 2: Interrupt Handling

* Interrupt Descriptor Table (IDT) is populated with handler entries.
* External interrupts enabled include:
  * Keyboard (for user input)
  * Timer (for preemptive multitasking)
  * Disk controller (for block IO)

### Step 3: Processes and Scheduling

* Each process's memory is isolated using a local descriptor with a 64 MB limit.
* A new page is allocated for each process's `task_struct`. This structure has a `tss_struct` at the bottom (storing process state) and a kernel stack at the top. Interrupt handlers share these kernel stacks.
* Regular timer interrupts schedules the processes based on time slices.
* Wait lists (implemented as linked lists on different kernel stacks) allow blocked syscalls to put themselves to sleep and yield the CPU by calling `schedule()`.


