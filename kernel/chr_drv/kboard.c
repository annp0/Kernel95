#include <kernel/sched.h>
#include <asm/system.h>
#include <asm/io.h>

static char key_map[0x7f] = {
    0, 27,
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
    127, 9,
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
    10, 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
    '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0, '*', 0, 32,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '-', 0, 0, 0, '+',
    0,0,0,0,0,0,0,
    '>',
    0,0,0,0,0,0,0,0,0,0
};

static char shift_map[0x7f] = {
    0,27,
    '!', '@', '#', '$', '%', '^', '&', '*','(',')','_','+',
    127,9,
    'Q','W','E','R','T','Y','U','I','O','P','{','}',
    10,0,
    'A','S','D','F','G','H','J','K','L',':','\"',
    '~',0,
    '|','Z','X','C','V','B','N','M','<','>','?',
    0,'*',0,32,                                  //36h-39h
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,             //3Ah-49h
    '-',0,0,0,'+',                               //4A-4E
    0,0,0,0,0,0,0,                               //4F-55
    '>',
    0,0,0,0,0,0,0,0,0,0
};

unsigned char scan_code, leds, mode, e0;

void keyboard_handler(void) {
    unsigned char a, scan_code;
    
    scan_code = inb_p(0x60);
    outb(0x20, 0x20);

    if (scan_code == 0xE0) {
        e0 = 1;
    }
    else if (scan_code == 0xE1) {
        e0 = 2;
    }
    else {
        if (scan_code >= 128)
            return;

        char c = key_map[scan_code];
        if (c == 0)
            return;

        printk("%c", c);
    }

    return;
}