#include <lib/types.h>
#include <lib/memory.h>
#include <kernel/idt.h>

idt_entry_t idt[IDT_TABLE_SIZE];
idt_ptr_t idtp;

void idt_set_gate(uint8_t index, uint32_t base, uint16_t selector, uint8_t flags)
{

    idt[index].baseLow = (base & 0xFFFF);
    idt[index].baseHigh = (base >> 16) & 0xFFFF;
    idt[index].selector = selector;
    idt[index].zero = 0;
    idt[index].flags = flags;

}

void idt_init()
{

    idtp.base = (uint32_t)&idt;
    idtp.limit = (sizeof (idt_entry_t) * IDT_TABLE_SIZE) - 1;

    memory_set(&idt, 0, sizeof (idt_entry_t) * IDT_TABLE_SIZE);

    idt_flush();

}

