#ifndef _TTY_H
#define _TTY_H

void con_init();
void tty_init();
void tty_write(unsigned channel, char* buf, int nr);
void console_print(const char* buf, int nr);

#endif
