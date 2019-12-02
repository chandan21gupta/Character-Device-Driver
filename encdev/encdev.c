#include <linux/fs.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/stat.h>
#include <linux/fcntl.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "encdev"
#define BLOCKSIZE 16
#define BUFFER_SIZE 256

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
//static ssize_t dev_write(struct file *, char *, size_t, loff_t *);

static ssize_t dev_write(struct file *, char *, size_t, loff_t *);

static struct file_operations dev_operations = {
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write
};

int reg;

static char buffer[BUFFER_SIZE] = {};
static int writePtr = 0;

int init_module(void) {

    reg = register_chrdev(0,DEVICE_NAME,&dev_operations);

    if(reg < 0)
        printk("encdev ERROR : Device not getting registered\n");

    else
        printk("encdev SUCCESS : Device successfully registered\n");
    return reg;
}

void cleanup_void(void) {

    unregister_chrdev(reg,DEVICE_NAME);
    printk("encdev SUCCESS : Device successfully unregistered\n");

}


static int dev_open(struct inode *ino, struct file *fil) {

    printk("decdev OPEN : Device successfully opened\n");
    return 0;
}

static int dev_release(struct inode *ino, struct file *fil) {

    printk("encdev RELEASE : Device successfully released\n");
    return 0;
}

static ssize_t dev_read(struct file *fil, char *data, size_t data_len, loff_t *t) {

    int i = 0;
    int j = 0;

    int count = 0;

    char transfer[data_len];

    for(i = 0; i < writePtr; i++) {
        transfer[i] = buffer[i];
    }
    if(writePtr < data_len) {
        transfer[writePtr] = '0';
    }
    j = copy_to_user(data,transfer,writePtr);
    if(j == 0)
        printk("encdev READ : data read successfully from the device\n");

    else 
        printk("encdev TRANSFER : error transfering data to reader\n");

    return writePtr;
}

static ssize_t dev_write(struct file *fil, char *data, size_t data_len, loff_t *t) {
    //int i = BLOCKSIZE;
    int k = 0;
    if(writePtr >= BUFFER_SIZE) {
        printk("encdev BUFFERSIZE : device capacity reached. Cannot store more data");
        return -1;
    }

    if(writePtr < BLOCKSIZE) {
        struct file *random_fd;
        mm_segment_t fs;
        random_fd = filp_open("/dev/urandom", O_RDONLY,0);
        if (random_fd == NULL){
            printk("encdev RANDOM : error in generating random key");
            return -2;
        }
        fs = get_fs();
        set_fs(get_fs());
        random_fd->f_op->read(random_fd,buffer,BLOCKSIZE,&random_fd->f_pos);
        set_fs(fs);

        writePtr += BLOCKSIZE;
        filp_close(random_fd,NULL);
    }

    while(data_len - writePtr <= BLOCKSIZE) {
        for(k = 0; k < BLOCKSIZE; k++) {
            buffer[writePtr+k] = buffer[writePtr+k-BLOCKSIZE]^data[writePtr+k-BLOCKSIZE];
        }
        writePtr += BLOCKSIZE;
    }

    printk("encdev WRITE : data written successfully to the device\n");
    return data_len;
}






