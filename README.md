# Character-Device-Driver

## Linux Kernel Module
Modules are pieces of code or software which can be loaded or unloaded during runtime in the kernel. This is very useful because now there is no need to add a piece of code in the kernel image and rebuild the kernel, which is quite troublesome.

## Device Drivers
Kernel Module Programming helps in adding device drivers for various devices without having to rebuild the kernel.

## Commands
1) go into encdev and decdev directory and run sudo make.<br>
2) mknod sudo mknod /dev/encdev c 150 1.<br>
3) chmod a+r+w /dev/encdev.<br>
4) repeat steps 2 and 3 for making device file for decdev device.<br>

## Implementation
The basic idea was to use file_operation type sturcture for custom file operations for the device file. The VFS file system internally handles the pointer to the original function definition. The init_module function module loads the module and while loading, it registers and returns the Major Number of the device driver.
Thereafter, the read and write system calls are used for reading and writing from and to the device to the user program.
The Encryption and Decryption algorithms were implemented based on the write up in the problem statement.
The buffer array contains the encrypted data and original array contains the decrypted data a.k.a original data.
Global Variables readPtr and writePtr are used to point to the current reading and writing pointers of the device file.

