#include <kernel/tty.h>

#define ORIG_VIDEO_PAGE     (*(unsigned short *)0x90004)
#define ORIG_VIDEO_MODE     ((*(unsigned short *)0x90006) & 0xff)
#define ORIG_VIDEO_COLS     (((*(unsigned short *)0x90006) & 0xff00) >> 8)
#define ORIG_VIDEO_LINES    ((*(unsigned short *)0x9000e) & 0xff)
#define ORIG_VIDEO_EGA_AX   (*(unsigned short *)0x90008)
#define ORIG_VIDEO_EGA_BX   (*(unsigned short *)0x9000a)
#define ORIG_VIDEO_EGA_CX   (*(unsigned short *)0x9000c)

#define VIDEO_TYPE_EGAC     0x21    /* EGA/VGA in Color Mode    */

static unsigned char    video_type;     /* Type of display being used   */
static unsigned long    video_num_columns;  /* Number of text columns   */
static unsigned long    video_num_lines;    /* Number of test lines     */
static unsigned long    video_mem_base;     /* Base of video memory     */
static unsigned long    video_mem_term;     /* End of video memory      */
static unsigned long    video_size_row;     /* Bytes per row        */
static unsigned char    video_page;     /* Initial video page       */
static unsigned short   video_port_reg;     /* Video register select port   */
static unsigned short   video_port_val;     /* Video register value port    */

void con_init() {
    char * display_desc = "EGAc";
    char * display_ptr;

    video_num_columns = ORIG_VIDEO_COLS;
    video_size_row = video_num_columns * 2;
    video_num_lines = ORIG_VIDEO_LINES;
    video_page = ORIG_VIDEO_PAGE;

    video_mem_base = 0xb8000;
    video_port_reg  = 0x3d4;
    video_port_val  = 0x3d5;

    video_type = VIDEO_TYPE_EGAC;
    video_mem_term = 0xc0000;

    display_ptr = ((char *)video_mem_base) + video_size_row - 8;
    while (*display_desc) {
        *(display_ptr++) = *(display_desc++);
        display_ptr++;
    }
}