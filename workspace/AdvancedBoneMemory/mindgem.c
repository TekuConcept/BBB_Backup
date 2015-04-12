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


#define DEVICE_NAME "mindgem"
struct cdev *mcdev;         // our character device
int major_number;           // will store our major number extracted from dev_t
int ret;                    // will be used to hold return values of functions
struct dev_buffer {
	char data[12];
	struct semaphore sem;
} vdev_buffer;
dev_t dev_num;              // will hold major number that the kernel gives us



// open file and lock buffer
int device_open(struct inode* inode, struct file* filp)
{
	if(down_interruptible(&vdev_buffer.sem) != 0)
	{
		printk(KERN_ALERT "mem_access: could not lock data buffer during open\n");
		return -1;
	}
	printk(KERN_INFO "mem_access: opened device\n");
	return 0;
}

// write buffer result to user space
ssize_t device_read(struct file* filp, char* bufStoreData, size_t bufCount, loff_t* curOffset)
{
	printk(KERN_INFO "mem_access: Reading from device\n");
	ret = copy_to_user(bufStoreData, vdev_buffer.data, bufCount);
	return ret;
}

// read into buffer from user space
ssize_t device_write(struct file* filp, const char* bufSourceData, size_t bufCount, loff_t* curOffset)
{
	printk(KERN_INFO "mem_access: writing to device\n");
	ret = copy_from_user(vdev_buffer.data, bufSourceData, bufCount);
	return ret;
}

// unlock buffer and close file
int device_close(struct inode* inode, struct file* filp)
{
	up(&vdev_buffer.sem);
	printk(KERN_INFO "mem_access: closed device\n");
	return 0;
}

// file operations description for device
struct file_operations fops = {
	.owner   = THIS_MODULE, // prevent unloading of this module when operations are in use
	.open    = device_open, // method pointer for opening the device
	.release = device_close,// method pointer for closing the device
	.write   = device_write,// method pointer for writing to the device
	.read    = device_read  // method pointer for reading from the device
};




// driver initialization
static int __init driver_entry(void)
{
	printk(KERN_INFO "Initializing mem_access...\t");
	ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
	if(ret < 0)
	{
		printk(KERN_ALERT "mem_access: failed to allocate major number\n");
		return ret;
	}
	major_number = MAJOR(dev_num);
	printk(KERN_INFO "mem_access: major number is %d", major_number);
	printk(KERN_INFO "\tuse \"mknod /dev/%s c %d 0\" for device file\n", DEVICE_NAME, major_number);

	mcdev = cdev_alloc();
	mcdev->ops = &fops;
	mcdev->owner = THIS_MODULE;

	ret = cdev_add(mcdev, dev_num, 1);
	if(ret < 0)
	{
		printk(KERN_ALERT "mem_access: unable to add cdev to kernel\n");
		return ret;
	}
	sema_init(&vdev_buffer.sem, 1);

	printk(KERN_INFO "Done!\n");
	return 0;
}

// driver deinitialization
static void __exit driver_exit(void)
{
	cdev_del(mcdev);

	unregister_chrdev_region(dev_num, 1);
	printk(KERN_ALERT "mem_access: unloaded module\n");
}



module_init(driver_entry);
module_exit(driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TekuConcept");
MODULE_DESCRIPTION("Exposes privledged access to physical memory.");
