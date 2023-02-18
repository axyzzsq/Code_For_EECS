#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>
#include <linux/videodev2.h>

#define MY_DEVICE_NAME "mydevice"
#define MY_DEVICE_CLASS "mydevice_class"

static dev_t my_dev;
static struct cdev my_cdev;
static struct class *my_class;

static long my_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {
    struct v4l2_capability cap;
    int ret;

    switch (cmd) {
        case VIDIOC_QUERYCAP:
            memset(&cap, 0, sizeof(struct v4l2_capability));
            strcpy(cap.driver, "mydriver");
            strcpy(cap.card, "mycard");
            strcpy(cap.bus_info, "mybusinfo");
            cap.capabilities = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;
            ret = copy_to_user((void __user *)arg, &cap, sizeof(struct v4l2_capability));
            if (ret) {
                return -EFAULT;
            }
            return 0;
        default:
            return -ENOTTY;
    }
}

static struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = my_ioctl,
};

static int __init my_init(void) {
    int ret;

    ret = alloc_chrdev_region(&my_dev, 0, 1, MY_DEVICE_NAME);
    if (ret < 0) {
        return ret;
    }

    cdev_init(&my_cdev, &my_fops);
    my_cdev.owner = THIS_MODULE;
    my_cdev.ops = &my_fops;

    ret = cdev_add(&my_cdev, my_dev, 1);
    if (ret < 0) {
        unregister_chrdev_region(my_dev, 1);
        return ret;
    }

    my_class = class_create(THIS_MODULE, MY_DEVICE_CLASS);
    if (IS_ERR(my_class)) {
        cdev_del(&my_cdev);
        unregister_chrdev_region(my_dev, 1);
        return PTR_ERR(my_class);
    }

    device_create(my_class, NULL, my_dev, NULL, MY_DEVICE_NAME);
    return 0;
}

	static void __exit my_exit(void) {
        device_destroy(my_class, my_dev);
        class_destroy(my_class);
        cdev_del(&my_cdev);
        unregister_chrdev_region(my_dev, 1);

}

    module_init(my_init);
    module_exit(my_exit);

    MODULE_LICENSE("Dual MIT/GPL");