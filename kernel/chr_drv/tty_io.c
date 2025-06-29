#include <kernel/tty.h>

struct tty_queue read_q = {0, 0, 0, 0, ""};
struct tty_queue write_q = {0, 0, 0, 0, ""};

unsigned long CHARS(struct tty_queue* q) {
    return (q->head - q->tail) & (TTY_BUF_SIZE - 1);
}

void PUTCH(char c, struct tty_queue* q) {
    q->buf[q->head++] = c;
    q->head &= (TTY_BUF_SIZE - 1);
}

char GETCH(struct tty_queue* q) {
    char c = q->buf[q->tail++];
    q->tail &= (TTY_BUF_SIZE - 1);
    return c;
}

char EMPTY(struct tty_queue* q) {
    return q->tail == q->head;
}

void tty_init() {
    read_q = (struct tty_queue){0, 0, 0, 0, ""};
    write_q = (struct tty_queue){0, 0, 0, 0, ""};

    con_init();
}

void tty_write(unsigned channel, char* buf, int nr) {
    con_print(buf, nr);
}

void copy_to_cooked() {
    signed char c;

    while (!EMPTY(&read_q)) {
        c = GETCH(&read_q);
        if (c == 10) {
            PUTCH(10, &write_q);
            PUTCH(13, &write_q);
        }
        else if (c == 8 || c == 9 || c == 13) {
            PUTCH(c, &write_q);
        }
        else if (c < 32) {
            PUTCH('^', &write_q);
            PUTCH(c + 64, &write_q);
        }
        else
            PUTCH(c, &write_q);

        con_write();
    }
}

void do_tty_interrupt() {
    copy_to_cooked();
}