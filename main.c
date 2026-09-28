#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
    int fbfd = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long screensize = 0;
    char *fbp = 0;
    
    // 1. Open the framebuffer device
    fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        return 1;
    }

    // 2. Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        close(fbfd);
        return 1;
    }

    // 3. Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        close(fbfd);
        return 1;
    }

    printf("Display active: %dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    // 4. Figure out the size of the screen in bytes
    screensize = vinfo.yres_virtual * finfo.line_length;

    // 5. Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((long)fbp == -1) {
        perror("Error: failed to map framebuffer device to memory");
        close(fbfd);
        return 1;
    }

    // 6. Draw a direct pixel pattern (a gradient rectangle)
    for (int y = 0; y < vinfo.yres; y++) {
        for (int x = 0; x < vinfo.xres; x++) {
            long location = (x * (vinfo.bits_per_pixel / 8)) + (y * finfo.line_length);
            
            if (vinfo.bits_per_pixel == 32) {
                // BGRA or RGBA layout depending on VM setup
                *((unsigned char *)(fbp + location + 0)) = (x * 255) / vinfo.xres; // Blue / Red channel
                *((unsigned char *)(fbp + location + 1)) = (y * 255) / vinfo.yres; // Green channel
                *((unsigned char *)(fbp + location + 2)) = 128;                     // Red / Blue channel
                *((unsigned char *)(fbp + location + 3)) = 255;                     // Alpha
            }
        }
    }

    printf("Painted pixels directly to /dev/fb0! Holding for 5 seconds...\n");
    sleep(5);

    // 7. Cleanup
    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}
