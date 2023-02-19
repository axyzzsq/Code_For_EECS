#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

int main(int argc, char *argv[]) {
    int fd;
    struct v4l2_capability cap;
    int ret;

    fd = open("/dev/mydevice", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return -1;
    }

    ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
    if (ret < 0) {
        perror("Failed to query capabilities");
        return -1;
    }

    printf("Driver: %s\n", cap.driver);
    printf("Card: %s\n", cap.card);
    printf("Bus info: %s\n", cap.bus_info);
    printf("Capabilities: %08x\n", cap.capabilities);

    close(fd);
    return 0;
}
