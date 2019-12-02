#include <linux/fs.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/stat.h>
#include <linux/fcntl.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "decdev"
#define BUFFER_SIZE 256
#define BLOCKSIZE 16

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, char *, size_t, loff_t *);

static struct file_operations dev_operations = {
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write
};

int reg;

//static char buffer;
char original[BUFFER_SIZE] = {};
//static int readPtr = 0;
static int writePtr = 0;

int init_module(void) {

    reg = register_chrdev(0,DEVICE_NAME,&dev_operations);

    if(reg < 0)
        printk("decdev ERROR : Device not getting registered\n");

    else
        printk("decdev SUCCESS : Device successfully registered\n");
    return reg;
}

void cleanup_void(void) {

    unregister_chrdev(reg,DEVICE_NAME);
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

static ssize_t dev_read(struct file *fil, char *data, size_t data_len, loff_t *t) {
    int i = 0;
    int j = 0;

    int count = 0;

    char transfer[data_len];

    for(i = 0; i < writePtr; i++) {
        transfer[i] = original[i];
    }
    if(writePtr < data_len) {
        transfer[writePtr] = '0';
    }
    j = copy_to_user(data,transfer,writePtr);
    if(j == 0)
        printk("decdev READ : data read successfully from the device\n");

    else 
        printk("decdev TRANSFER : error transfering data to reader\n");

    return writePtr;
}

static ssize_t dev_write(struct file *fil, char *data, size_t data_len, loff_t *t) {

    //int i = 0;
    int k = 0;

    if(writePtr >= BUFFER_SIZE) {
        printk("decdev BUFFERSIZE : device capacity reached. Cannot store more data");
        return -1;
    }
    // return BLOCKSIZE;
    while(writePtr < BLOCKSIZE) {
       
       original[writePtr] = data[writePtr];
       writePtr++;
       //writePtr += BLOCKSIZE;
    }
    while(data_len - writePtr <= BLOCKSIZE) {
        for(k = 0; k < BLOCKSIZE; k++) {
            original[writePtr+k] = data[writePtr+k-BLOCKSIZE]^data[writePtr+k];
        }
        writePtr += BLOCKSIZE;
    }

    printk("decdev WRITE : data written successfully to the device\n");
    return data_len;
}



