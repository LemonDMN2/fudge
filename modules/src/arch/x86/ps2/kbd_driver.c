#include <memory.h>
#include <modules/modules.h>
#include <arch/x86/apic/apic.h>
#include <arch/x86/io/io.h>
#include <arch/x86/ps2/ps2.h>

static char mapUS[256] =
{
       0,   27,  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',  '-',  '+', '\b', '\t',
     'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',  'o',  'p',  '[',  ']', '\n',    0,  'a',  's',
     'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';', '\'',  '`',    0, '\\',  'z',  'x',  'c',  'v',
     'b',  'n',  'm',  ',',  '.',  '/',    0,    0,    0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
       0,   27,  '!',  '@',  '#',  '$',  '%',  '^',  '&',  '*',  '(',  ')',  '_',  '=', '\b', '\t',
     'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',  'O',  'P',  '{',  '}', '\n',    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':', '\"',  '~',    0,  '|',  'Z',  'X',  'C',  'V',
     'B',  'N',  'M',  '<',  '>',  '?',    0,    0,    0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0
};

static unsigned int buffer_read(struct ps2_kbd_buffer *self, unsigned int count, char *buffer)
{

    unsigned int i;

    for (i = 0; i < count; i++)
    {

        unsigned int tail = (self->tail + 1) % self->size;

        if (self->head == self->tail)
            break;

        buffer[i] = self->buffer[self->tail];
        self->tail = tail;

    }

    return i;

}

static unsigned int buffer_write(struct ps2_kbd_buffer *self, unsigned int count, char *buffer)
{

    unsigned int i;

    for (i = 0; i < count; i++)
    {

        unsigned int head = (self->head + 1) % self->size;

        if (head == self->tail)
            break;

        self->buffer[self->head] = buffer[i];
        self->head = head;

    }

    return i;

}

static void handle_irq(struct modules_device *self)
{

    unsigned int scancode;
    struct ps2_kbd_driver *kbd = (struct ps2_kbd_driver *)self->driver;

    if (!kbd)
        return;

    scancode = io_inb(PS2_REGISTER_DATA);

    if (kbd->escaped)
    {

        kbd->escaped = 0;

    }

    if (scancode == 0xE0)
        kbd->escaped = 1;

    if (scancode == 0x38)
        kbd->toggleAlt = 1;

    if (scancode == 0xB8)
        kbd->toggleAlt = 0;

    if (scancode == 0x1D)
        kbd->toggleCtrl = 1;

    if (scancode == 0x9D)
        kbd->toggleCtrl = 0;

    if (scancode == 0x2A)
        kbd->toggleShift = 1;

    if (scancode == 0xAA)
        kbd->toggleShift = 0;

    if (scancode & 0x80)
    {

    }

    else
    {

        if (kbd->toggleShift)
            scancode += 128;

        kbd->buffer.write(&kbd->buffer, 1, &kbd->map[scancode]);

    }

}

static void attach(struct modules_device *device)
{

    struct ps2_kbd_device *kbdDevice = (struct ps2_kbd_device *)device;

    apic_register_routine(kbdDevice->irq, device, handle_irq);

}

static unsigned int check(struct modules_driver *self, struct modules_device *device)
{

    return device->type == PS2_KBD_DEVICE_TYPE;

}

void ps2_kbd_driver_init(struct ps2_kbd_driver *driver)
{

    memory_clear(driver, sizeof (struct ps2_kbd_driver));

    modules_driver_init(&driver->base, PS2_KBD_DRIVER_TYPE, "kbd", 0, check, attach);

    driver->buffer.size = 256;
    driver->buffer.read = buffer_read;
    driver->buffer.write = buffer_write;
    driver->map = mapUS;

}

