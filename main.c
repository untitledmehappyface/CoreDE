#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
    int fbfd = open("/dev/fb0", O_RDWR);
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo);
    ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo);

    long screensize = vinfo.yres_virtual * finfo.line_length;
    char *fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);

    // 1. Fill screen with sleek dark grey/black background
    for (long i = 0; i < screensize; i += 4) {
        fbp[i] = 20;   // Blue
        fbp[i+1] = 20; // Green
        fbp[i+2] = 20; // Red
        fbp[i+3] = 255;
    }

    // 2. Draw a centered loading bar outline / box
    int start_x = vinfo.xres / 2 - 150;
    int end_x = vinfo.xres / 2 + 150;
    int start_y = vinfo.yres / 2 - 20;
    int end_y = vinfo.yres / 2 + 20;

    for (int y = start_y; y <= end_y; y++) {
        for (int x = start_x; x <= end_x; x++) {
            long loc = (x * (vinfo.bits_per_pixel / 8)) + (y * finfo.line_length);
            // Border color: Cyan
            if (y == start_y || y == end_y || x == start_x || x == end_x) {
                fbp[loc] = 255; fbp[loc+1] = 255; fbp[loc+2] = 0;
            } 
            // Fill animation simulation: progress fill
            else if (x < start_x + 120) {
                fbp[loc] = 255; fbp[loc+1] = 128; fbp[loc+2] = 0; // Orange progress
            }
        }
    }

    sleep(3); // Show boot screen for 3 seconds
    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}
