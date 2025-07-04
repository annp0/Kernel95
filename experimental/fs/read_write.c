#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>

#include <linux/kernel.h>
#include <linux/sched.h>

extern int file_read(struct m_inode * inode, struct file * filp,
        char * buf, int count);
extern int file_write(struct m_inode * inode, struct file * filp,
        char * buf, int count);

extern int rw_char(int rw,int dev, char * buf, int count, off_t * pos);

int sys_read(unsigned int fd,char * buf,int count) {
    struct file * file;
    struct m_inode * inode;

    if (fd>=NR_OPEN || count<0 || !(file=current->filp[fd]))
        return -EINVAL;
    if (!count)
        return 0;

    inode = file->f_inode;
    if (S_ISCHR(inode->i_mode)) {
        return rw_char(READ, inode->i_zone[0], buf, count, &file->f_pos);
    }

    if (S_ISDIR(inode->i_mode) || S_ISREG(inode->i_mode)) {
        if (count+file->f_pos > inode->i_size)
            count = inode->i_size - file->f_pos;
        if (count<=0)
            return 0;
        return file_read(inode,file,buf,count);
    }

    printk("(Read)inode->i_mode=%06o\n\r",inode->i_mode);
    return -EINVAL;
}

int sys_write(unsigned int fd,char * buf,int count) {
    struct file * file;
    struct m_inode * inode;

    if (fd>=NR_OPEN || count <0 || !(file=current->filp[fd]))
        return -EINVAL;

    if (!count)
        return 0;

    inode=file->f_inode;

    if (S_ISCHR(inode->i_mode)) {
        return rw_char(WRITE,inode->i_zone[0],buf,count,&file->f_pos);
    }

    if (S_ISREG(inode->i_mode))
        return file_write(inode,file,buf,count);

    printk("(Write)inode->i_mode=%06o\n\r",inode->i_mode);
    return -EINVAL;
}

