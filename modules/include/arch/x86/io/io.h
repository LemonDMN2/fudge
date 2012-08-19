#ifndef MODULES_IO_H
#define MODULES_IO_H

volatile unsigned char io_inb(unsigned short port);
volatile unsigned short io_inw(unsigned short port);
volatile unsigned int io_ind(unsigned short port);
void io_outb(unsigned short port, unsigned char data);
void io_outw(unsigned short port, unsigned short data);
void io_outd(unsigned short port, unsigned int data);

#endif

