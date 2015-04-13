/**
  * Base code provided by SolidusCode
  * <https://www.youtube.com/channel/UCYXWGO7hi4McH2qRLWq1dIQ>
  *
  * Created: Apr 12, 2015
  *      by: TekuConcept
  *
  *
  * Load with insmod, then follow instructions from dmesg
  */

#include <linux/module.h>   //
#include <linux/kernel.h>   //
#include <linux/init.h>     // initializations macros

#include <linux/fs.h>       // file_operations structure
#include <linux/cdev.h>     // char driver - cdev
#include <linux/semaphore.h>// used to access semaphores synchronization behaviors
#include <asm/uaccess.h>    // copy_to_user; copy_from_user

#include <asm/io.h>         // ioremap, iounmap


#define DEVICE_NAME "mindgem"
struct cdev *mcdev;         // our character device
int major_number;           // will store our major number extracted from dev_t
int ret;                    // will be used to hold return values of functions
struct dev_buffer {
	char data[32];
	struct semaphore sem;
} vdev_buffer;
dev_t dev_num;              // will hold major number that the kernel gives us

uint32_t* pmem;             // mapped memory region
char flag;                  // flag to determine actions
uint32_t addr;              // stored address
uint16_t offs;              // stored offset
uint32_t val;               // value used for writes


/* ------------------------------------------------------------------ *\
|                          FILE OPERATIONS                             |
\* ------------------------------------------------------------------ */

int device_open(struct inode* inode, struct file* filp)
{
	if(down_interruptible(&vdev_buffer.sem) != 0)
	{
		printk(KERN_ALERT "mindgem: could not lock data buffer during open\n");
		return -1;
	}
	printk(KERN_INFO "mindgem: opened device\n");
	return 0;
}

// write to user: user performs read action
ssize_t device_read(struct file* filp, char* bufStoreData, size_t bufCount, loff_t* curOffset)
{
	printk(KERN_INFO "mindgem: Reading from device\n");

	// break down read value into characters
	if(pmem != 0)
		val = pmem[offs];
	vdev_buffer.data[0] = (char)(val >> 24);
	vdev_buffer.data[1] = (char)(val >> 16);
	vdev_buffer.data[2] = (char)(val >>  8);
	vdev_buffer.data[3] = (char)(val);

	ret  = copy_to_user(bufStoreData, vdev_buffer.data, bufCount);
	return ret;
}

// read from user: user performs a write action
ssize_t device_write(struct file* filp, const char* bufSourceData, size_t bufCount, loff_t* curOffset)
{
	printk(KERN_INFO "mindgem: writing to device\n");
	val = 0; // reset to 0

	// get input from user
	// parse input into fields: [flag][address][offset][value]
	// flags: 'r' for read, 'w' for write, 'z' for offset change
	ret = copy_from_user(vdev_buffer.data, bufSourceData, bufCount);

	// store flag and proceed
	flag = vdev_buffer.data[0];

	// if read or write, get new address and offset
	if(flag == 'r' || flag == 'R' || flag == 'w' || flag == 'W')
	{
		// clear
		addr = 0;
		offs = 0;

		// parse
		addr = (vdev_buffer.data[1] << 24) | (vdev_buffer.data[2] << 16) | (vdev_buffer.data[3] << 8) | vdev_buffer.data[4];
		offs = (vdev_buffer.data[5] << 8) | vdev_buffer.data[6];
		offs /= 4;

		// unmap old and remap new
		if(pmem != 0)
			iounmap(pmem);
		pmem = ioremap(addr, 0xFFF);
	}
	// if if sleep, get new offset
	else if('z')
	{
		offs = (vdev_buffer.data[1] << 8) | vdev_buffer.data[2];
		return ret;
	}

	// if write, get write value and write to register
	if(flag == 'w')
	{
		// parse
		val = (vdev_buffer.data[7] << 24) | (vdev_buffer.data[8] << 16) | (vdev_buffer.data[9] << 8) | vdev_buffer.data[10];

		// write
		if(pmem == 0)
		{
			printk(KERN_INFO "mindgem: no memory space to write to...\n");
		}
		else
		{
			pmem[offs] = val;
		}
	}

	return ret;
}

// user closes the file
int device_close(struct inode* inode, struct file* filp)
{
	if(pmem != 0)
		iounmap(pmem);

	up(&vdev_buffer.sem);
	printk(KERN_INFO "mindgem: closed device\n");
	return 0;
}

struct file_operations fops = {
	.owner   = THIS_MODULE,	// prevent unloading of this module when operations are in use
	.open    = device_open,	// points to the method to call when opening the device
	.release = device_close,// points to the method to call when closing the device
	.write   = device_write,// points to the method to call when writing to the device
	.read    = device_read	// points to the method to call when reading from the device
};





/* ------------------------------------------------------------------ *\
|                              MAIN MODULE                             |
\* ------------------------------------------------------------------ */

static int __init driver_entry(void)
{
	printk(KERN_INFO "Initializing mindgem...\t");

	ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
	if(ret < 0)
	{
		printk(KERN_ALERT "mindgem: failed to allocate major number\n");
		return ret;
	}
	major_number = MAJOR(dev_num);
	printk(KERN_INFO "mindgem: major number is %d", major_number);
	printk(KERN_INFO "\tuse \"mknod /dev/%s c %d 0\" for device file\n", DEVICE_NAME, major_number);

	mcdev = cdev_alloc();
	mcdev->ops = &fops;
	mcdev->owner = THIS_MODULE;

	ret = cdev_add(mcdev, dev_num, 1);
	if(ret < 0)
	{
		printk(KERN_ALERT "mindgem: unable to add cdev to kernel\n");
		return ret;
	}
	sema_init(&vdev_buffer.sem, 1);

	printk(KERN_INFO "Done!\n");
	return 0;
}

static void __exit driver_exit(void)
{
	cdev_del(mcdev);

	unregister_chrdev_region(dev_num, 1);
	printk(KERN_ALERT "mindgem: unloaded module\n");
}



module_init(driver_entry);
module_exit(driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TekuConcept");
MODULE_DESCRIPTION("Exposes privledged physical memory to user space.");
