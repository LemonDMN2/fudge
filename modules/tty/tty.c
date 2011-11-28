#include <lib/memory.h>
#include <lib/string.h>
#include <kernel/vfs.h>
#include <kernel/modules.h>
#include <modules/vga/vga.h>
#include <modules/kbd/kbd.h>
#include <modules/stream/stream.h>
#include <modules/tty/tty.h>

static struct tty_device device;

static void clear()
{

    char c = ' ';
    int i;

    for (i = 0; i < TTY_CHARACTER_SIZE; i++)
        device.vgaDevice->write_framebuffer(i, 1, &c);

}

static void scroll()
{

    char buffer[TTY_CHARACTER_SIZE];

    device.vgaDevice->read_framebuffer(TTY_CHARACTER_WIDTH, TTY_CHARACTER_SIZE - TTY_CHARACTER_WIDTH, buffer);

    unsigned int i;

    for (i = TTY_CHARACTER_SIZE - TTY_CHARACTER_WIDTH; i < TTY_CHARACTER_SIZE; i++)
        buffer[i] = ' ';

    device.vgaDevice->write_framebuffer(0, TTY_CHARACTER_SIZE, buffer);

    device.cursorOffset -= TTY_CHARACTER_WIDTH;

}

static void putc(char c)
{

    if (c == '\b')
    {

        device.cursorOffset--;

    }

    else if (c == '\t')
    {

        device.cursorOffset = (device.cursorOffset + 8) & ~(8 - 1);

    }

    else if (c == '\r')
    {

        device.cursorOffset -= (device.cursorOffset % TTY_CHARACTER_WIDTH);

    }

    else if (c == '\n')
    {

        device.cursorOffset += TTY_CHARACTER_WIDTH - (device.cursorOffset % TTY_CHARACTER_WIDTH);

    }
    
    else if (c >= ' ')
    {

        device.vgaDevice->write_framebuffer(device.cursorOffset, 1, &c);
        device.cursorOffset++;

    }

    if (device.cursorOffset >= TTY_CHARACTER_WIDTH * TTY_CHARACTER_HEIGHT)
        scroll();

}

static unsigned int tty_device_cwd_read(struct vfs_node *self, unsigned int count, void *buffer)
{

    count = string_length(device.cwdname);

    string_write(buffer, device.cwdname);

    return count;

}

static unsigned int tty_device_cwd_write(struct vfs_node *self, unsigned int count, void *buffer)
{

    count = string_length(buffer);

    string_write(device.cwdname, buffer);

    return count;

}

static unsigned int tty_device_in_read(struct vfs_node *self, unsigned int count, void *buffer)
{

    unsigned int i;

    for (i = 0; i < count; i++)
    {

        char c;

        if (!device.kbdDevice->buffer.getc(&device.kbdDevice->buffer, &c))
            break;

        ((char *)buffer)[i] = c;

    }

    return i;

}

static unsigned int tty_device_out_write(struct vfs_node *self, unsigned int count, void *buffer)
{

    unsigned int i;

    for (i = 0; i < count; i++)
        putc(((char *)buffer)[i]);

    device.vgaDevice->set_cursor_offset(device.cursorOffset);

    return count;

}

static unsigned int tty_device_view_read(struct vfs_node *self, unsigned int count, void *buffer)
{

    unsigned int i;

    for (i = 0; i < VFS_FILESYSTEM_SLOTS; i++)
    {

        struct vfs_filesystem *filesystem = vfs_get_filesystem(i);

        if (!filesystem)
            continue;

        struct vfs_view *view = filesystem->find_view(filesystem, device.cwdname);

        if (!view)
            continue;

        unsigned int j;
        unsigned int start = 0;

        for (j = 0; j < 64; j++)
        {

            struct vfs_node *node = view->walk(view, j);

            if (!node)
                continue;

            string_write_format(buffer + start, "%s\n", node->name);
            start += string_length(node->name) + 1;

        }

    }

    return string_length(buffer);

}

void tty_device_init(struct tty_device *device, char *cwdname)
{

    modules_device_init(&device->base, TTY_DEVICE_TYPE);
    device->cursorOffset = 0;
    device->kbdDevice = (struct kbd_device *)modules_get_device(KBD_DEVICE_TYPE);
    device->vgaDevice = (struct vga_device *)modules_get_device(VGA_DEVICE_TYPE);
    device->vgaDevice->set_cursor_color(TTY_COLOR_WHITE, TTY_COLOR_BLACK);
    clear();

    string_write(device->cwdname, cwdname);

    stream_device_init(&device->in, "stdin", tty_device_in_read, 0);
    stream_device_init(&device->out, "stdout", 0, tty_device_out_write);
    stream_device_init(&device->error, "stderr", 0, tty_device_out_write);
    stream_device_init(&device->cwd, "cwd", tty_device_cwd_read, tty_device_cwd_write);
    stream_device_init(&device->view, "view", tty_device_view_read, 0);

}

void init()
{

    tty_device_init(&device, "home");

    modules_register_device(&device.in.base);
    modules_register_device(&device.out.base);
    modules_register_device(&device.error.base);
    modules_register_device(&device.cwd.base);
    modules_register_device(&device.view.base);
    modules_register_device(&device.base);

}

void destroy()
{

    modules_unregister_device(&device.in.base);
    modules_unregister_device(&device.out.base);
    modules_unregister_device(&device.error.base);
    modules_unregister_device(&device.cwd.base);
    modules_unregister_device(&device.view.base);
    modules_unregister_device(&device.base);

}

