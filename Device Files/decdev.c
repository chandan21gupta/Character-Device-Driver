#include <linux/fs.h>
#include <linux/module.h>
#include <uaccess.h>
#include <string.h>

#define DEVICE_NAME "decdev"
#define BUFFER_SIZE 256
#define BLOCK_SIZE 16

static struct file_operations dev_operations = {
    .open = dev_open,
    .release = dev_release
    .read = dev_read,
    .write = dev_write,
};

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, char *, size_t, loff_t *);

int reg;

extern static char buffer[BUFFER_SIZE] = {};
char original[BUFFER_SIZE] = {};
static int readPtr = 0;
static int writePtr = 0;

int init_module(void) {

    reg = register_chrdev(0,DEVICE_NAME,dev_operations);

    if(reg < 0)
        printk("decdev ERROR : Device not getting registered\n");

    else
        printk("decdev SUCCESS : Device successfully registered\n");

}

void cleanup_void(void) {

    unregister_dev(reg,DEVICE_NAME);
    printk("decdev SUCCESS : Device successfully unregistered\n");

}


static int dev_open(struct inode *ino, struct file *fil) {

    printk("decdev OPEN : Device successfully opened\n");
    return 0;
}

static int dev_release(struct inode *ino, struct file *fil) {

    printk("decdev RELEASE : Device successfully released\n");
    return 0;
}

static ssize_t dev_read(struct file *fil, char *data, ssize_t data_len, loff_t *t) {
    char transfer[BLOCK_SIZE];
    for(int i=0;i<BLOCK_SIZE;i++) {
        transfer[i] = original[readPtr+i];
    }
    readPtr += BLOCK_SIZE;
    int i = copy_to_user(data, transfer, BLOCK_SIZE);
    if(i == 0)
        printk("decdev READ : data read successfully from the device\n");
    return BLOCK_SIZE;
}

static ssize_t dev_write(struct file *fil, char *data, ssize_t data_len, loff_t *t) {

    if(writePtr < BLOCK_SIZE) {
       for(int i=0;i<BLOCK_SIZE;i++) {
            original[i] = buffer[i];
       }
       writePtr += 16;
    }
    else {
        for(int i=0;i<BLOCK_SIZE;i++) {
            original[i+writePtr] = buffer[i+writePtr-16]^buffer[i+writePtr];
        }
        writePtr += BLOCK_SIZE;
    }
    printk("decdev WRITE : data written successfully to the device\n");
    return 0;
}



