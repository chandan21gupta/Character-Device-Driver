#include <linux/fs.h>

#define DEVICE_NAME "encdev"

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

int init_module(void) {

    reg = register_chrdev(0,DEVICE_NAME,dev_operations);

    if(reg < 0)
        printk("ERROR : Device not getting registered\n");

    else
        printk("SUCCESS : Device successfully registered\n");

}

void cleanup_void(void) {

    unregister_dev(reg,DEVICE_NAME);
    printk("SUCCESS : Device successfully unregistered\n");

}


static int dev_open(struct inode *ino, struct file *fil) {

    return 0;
}

static int dev_release(struct inode *ino, struct file *fil) {

    return 0;
}

static ssize_t dev_read(struct file *fil, char *data, ssize_t data_len, loff_t *t) {

}

static ssize_t dev_write(struct file *fil, char *data, ssize_t data_len, loff_t *t) {

}



