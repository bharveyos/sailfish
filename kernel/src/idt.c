#include "idt.h"
#include "memutils.h"

#define IDT_MAX_DESCRIPTORS 256
static bool	 vectors[IDT_MAX_DESCRIPTORS];
extern void *isr_stub_table[];

__attribute__((aligned(16))) static idt_entry_t idt[256];
static idtr_t									idtr;

void exception_handler() { asm volatile("cli; hlt"); }

void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags)
{
	idt_entry_t *descriptor = &idt[vector];

	descriptor->isr_lb = (uint32_t)isr & 0xFFFF;
	descriptor->kernel_cs = 0x08;
	descriptor->attributes = flags;
	descriptor->isr_hb = (uint32_t)isr >> 16;
	descriptor->reserved = 0;
}

void idt_init()
{
	idtr.base = (uintptr_t)&idt[0];
	idtr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

	for (uint8_t vector = 0; vector < 32; vector++)
	{
		idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
		vectors[vector] = true;
	}

	asm volatile("lidt %0" : : "m"(idtr));
	asm volatile("sti");
}
