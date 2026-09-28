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

    // 1. Paint Desktop Background (Dark Navy/Slate Blue)
    for (int y = 0; y < vinfo.yres; y++) {
        for (int x = 0; x < vinfo.xres; x++) {
            long loc = (x * (vinfo.bits_per_pixel / 8)) + (y * finfo.line_length);
            fbp[loc]   = 60;  // Blue
            fbp[loc+1] = 40;  // Green
            fbp[loc+2] = 30;  // Red
            fbp[loc+3] = 255; // Alpha
        }
    }

    // 2. Draw Bottom Taskbar (Height: 40 pixels, Dark Charcoal color)
    int taskbar_height = 40;
    int start_y = vinfo.yres - taskbar_height;

    for (int y = start_y; y < vinfo.yres; y++) {
        for (int x = 0; x < vinfo.xres; x++) {
            long loc = (x * (vinfo.bits_per_pixel / 8)) + (y * finfo.line_length);
            
            // Top border line of the taskbar (bright accent line)
            if (y == start_y) {
                fbp[loc]   = 255; // Blue
                fbp[loc+1] = 128; // Green
                fbp[loc+2] = 0;   // Red
            } else {
                fbp[loc]   = 40;  // Taskbar background body
                fbp[loc+1] = 40;
                fbp[loc+2] = 40;
            }
            fbp[loc+3] = 255;
        }
    }

    // Keep it open for 5 seconds to check out the layout
    sleep(5);
    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}
