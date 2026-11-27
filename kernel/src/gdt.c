#include "gdt.h"
#include "memutils.h"

#define GDT_ENTRIES 7

gdt_entry_t gdt[GDT_ENTRIES];
gdtr_t		gdtr;
tss_entry_t tss;

extern void use_kgdt(gdtr_t *gdtr);
extern void reload_segments();

void set_gdt_entry(gdt_entry_t *gdt, uint64_t base, uint64_t limit, uint8_t access_byte, uint8_t granularity_byte)
{
	uint32_t base_low = (uint32_t)(base & 0xFFFFFFFF);

	gdt->base_hb = (uint8_t)((base >> 24) & 0xFF);
	gdt->granularity = granularity_byte;
	gdt->access_byte = access_byte;
	gdt->base_mb = (uint8_t)((base_low >> 16) & 0xFF);
	gdt->base_lb = (uint16_t)(base_low & 0xFFFF);
	gdt->limit_lb = (uint16_t)(limit & 0xFFFF);
}

void set_tss_descriptor(int position, uint64_t base, uint64_t limit)
{
	uint32_t base_low = (uint32_t)(base & 0xFFFFFFFF);
	uint32_t base_high = (uint32_t)(base >> 32);

	gdt[position].limit_lb = (uint16_t)(limit & 0xFFFF);
	gdt[position].base_lb = (uint16_t)(base_low & 0xFFFF);
	gdt[position].base_mb = (uint8_t)((base_low >> 16) & 0xFF);
	gdt[position].access_byte = 0x89; /* 64-bit TSS, present */
	gdt[position].granularity = 0x00;
	gdt[position].base_hb = (uint8_t)((base_low >> 24) & 0xFF);

	gdt[position + 1].limit_lb = (uint16_t)(base_high & 0xFFFF);
	gdt[position + 1].base_lb = 0;
	gdt[position + 1].base_mb = 0;
	gdt[position + 1].access_byte = 0x00; /* 64-bit TSS, present */
	gdt[position + 1].granularity = 0x00;
	gdt[position + 1].base_hb = 0;
}

void gdt_init(void)
{
	tss.iomap_base = sizeof(tss_entry_t);
	/* some other entries here */

	set_gdt_entry(&gdt[0], 0, 0, 0, 0);
	set_gdt_entry(&gdt[1], 0, 0xFFFFF, 0x9A, 0xAF);
	set_gdt_entry(&gdt[2], 0, 0xFFFFF, 0x92, 0xCF);
	set_gdt_entry(&gdt[3], 0, 0xFFFFF, 0xFA, 0xAF);
	set_gdt_entry(&gdt[4], 0, 0xFFFFF, 0xF2, 0xCF);
	set_tss_descriptor(5, (uint64_t)&tss, sizeof(tss_entry_t) - 1);

	gdtr.limit = (GDT_ENTRIES * 8) - 1;
	gdtr.base = (uint64_t)&gdt;

	use_kgdt(&gdtr);
}