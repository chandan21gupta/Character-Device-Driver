#include <linux/fs.h>
#include <linux/module.h>
#include <uaccess.h>
#include <string.h>

#define DEVICE_NAME "encdev"
#define BLOCK_SIZE 16
#define BUFFER_SIZE 256

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

static char buffer[BUFFER_SIZE] = {};
EXPORT_SYMBOL(buffer);
static int writePtr = 0;
static int readPtr = 0;

int init_module(void) {

    reg = register_chrdev(0,DEVICE_NAME,dev_operations);

    if(reg < 0)
        printk("encdev ERROR : Device not getting registered\n");

    else
        printk("encdev SUCCESS : Device successfully registered\n");

}

void cleanup_void(void) {

    unregister_dev(reg,DEVICE_NAME);
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

static ssize_t dev_read(struct file *fil, char *data, ssize_t data_len, loff_t *t) {
    char transfer[BLOCK_SIZE] = {};
    for(int i = 0; i < BLOCK_SIZE; i++) {
        transfer[i] = buffer[readPtr+i];
    }
    readPtr += BLOCK_SIZE;
    int i = copy_to_user(data,transfer,BLOCK_SIZE);
    if(i == 0)
        printk("encdev READ : data read successfully from the device\n");
    return BLOCK_SIZE
}

static ssize_t dev_write(struct file *fil, char *data, ssize_t data_len, loff_t *t) {

    if(writePtr >= 256) {
        printk("encdev BUFFERSIZE : device capacity reached. Cannot store more data");
        return -1;
    }

    if(writePtr < BLOCK_SIZE) {
        int random_fd = open("/dev/urandom", O_RDONLY);
        if (random_fd < 0){
            printk("encdev RANDOM : error in generating random key");
            return -2;
        }
        ssize_t result = read(random_fd, buffer, BLOCK_SIZE);
        if(result < 0){
            printk("encdev GENERATION : error while reading urandom");
            return -3;
        }
        writePtr += BLOCK_SIZE;
        close(random_fd);
    }
    else {
        char transfer[BLOCK_SIZE];
        int i = copy_from_user(transfer, data, BLOCK_SIZE);
        for(int i = 0;i<BLOCK_SIZE;i++) {
            buffer[writePtr+i] = transfer[i]^buffer[writePtr+i-16];
        }
        writePtr += 16;
    }
    printk("encdev WRITE : data written successfully to the device\n");
    return BLOCK_SIZE;
}



