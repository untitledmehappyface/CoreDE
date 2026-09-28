#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
    int fbfd = open("/dev/fb0", O_RDWR);
    int mousefd = open("/dev/input/event0", O_RDONLY | O_NONBLOCK); // Change event number if needed
    
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo);
    ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo);

    long screensize = vinfo.yres_virtual * finfo.line_length;
    char *fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);

    // Initial mouse position (center of screen)
    int mouse_x = vinfo.xres / 2;
    int mouse_y = vinfo.yres / 2;
    
    struct input_event ev;

    // Main desktop loop
    while (1) {
        // 1. Read mouse movement if available
        if (mousefd > 0 && read(mousefd, &ev, sizeof(struct input_event)) > 0) {
            if (ev.type == EV_REL) {
                if (ev.code == REL_X) mouse_x += ev.value;
                if (ev.code == REL_Y) mouse_y += ev.value;
                
                // Keep cursor on screen bounds
                if (mouse_x < 0) mouse_x = 0;
                if (mouse_x >= vinfo.xres - 10) mouse_x = vinfo.xres - 10;
                if (mouse_y < 0) mouse_y = 0;
                if (mouse_y >= vinfo.yres - 10) mouse_y = vinfo.yres - 10;
            }
        }

        // 2. Redraw Desktop Background & Taskbar (simplified)
        for (int y = 0; y < vinfo.yres - 40; y++) {
            for (int x = 0; x < vinfo.xres; x++) {
                long loc = (x * (vinfo.bits_per_pixel / 8)) + (y * finfo.line_length);
                fbp[loc] = 60; fbp[loc+1] = 40; fbp[loc+2] = 30; fbp[loc+3] = 255;
            }
        }

        // Taskbar
        for (int y = vinfo.yres - 40; y < vinfo.yres; y++) {
            for (int x = 0; x < vinfo.xres; x++) {
                long loc = (x * (vinfo.bits_per_pixel / 8)) + (y * finfo.line_length);
                fbp[loc] = 40; fbp[loc+1] = 40; fbp[loc+2] = 40; fbp[loc+3] = 255;
            }
        }

        // 3. Draw a simple white 10x10 cursor square at (mouse_x, mouse_y)
        for (int cy = 0; cy < 10; cy++) {
            for (int cx = 0; cx < 10; cx++) {
                int px = mouse_x + cx;
                int py = mouse_y + cy;
                if (px < vinfo.xres && py < vinfo.yres) {
                    long loc = (px * (vinfo.bits_per_pixel / 8)) + (py * finfo.line_length);
                    fbp[loc] = 255; fbp[loc+1] = 255; fbp[loc+2] = 255; fbp[loc+3] = 255;
                }
            }
        }

        usleep(10000); // ~100 FPS cap to save CPU
    }

    munmap(fbp, screensize);
    close(fbfd);
    if (mousefd > 0) close(mousefd);
    return 0;
}
