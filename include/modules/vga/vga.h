#ifndef MODULES_VGA_H
#define MODULES_VGA_H

#define VGA_FB_ADDRESS 0xB8000
#define VGA_FB_SIZE    2000

#define VGA_DEVICE_TYPE 3

struct vga_device
{

    struct modules_device base;
    unsigned char cursorColor;
    unsigned short cursorOffset;
    unsigned int (*read_framebuffer)(unsigned int offset, unsigned int count, void *buffer);
    unsigned int (*write_framebuffer)(unsigned int offset, unsigned int count, void *buffer);
    void (*set_cursor_color)(unsigned char fg, unsigned char bg);
    void (*set_cursor_offset)(unsigned short offset);

};

extern void vga_init();

#endif

