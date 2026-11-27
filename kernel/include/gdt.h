#ifndef GDT_H
#define GDT_H

#include <stdint.h>

typedef struct
{
	uint16_t limit_lb;
	uint16_t base_lb;
	uint8_t	 base_mb;
	uint8_t	 access_byte;
	uint8_t	 granularity;
	uint8_t	 base_hb;
} __attribute__((packed)) gdt_entry_t;

typedef struct
{
	uint32_t reserved0;
	uint64_t rsp[3]; /* RSP0, RSP1, RSP2 */
	uint64_t reserved1;
	uint64_t ist[7]; /* IST1..IST7 */
	uint64_t reserved2;
	uint16_t reserved3;
	uint16_t iomap_base;
} __attribute__((packed)) tss_entry_t;

typedef struct
{
	uint16_t limit;
	uint64_t base;
} __attribute__((packed)) gdtr_t;

void set_gdt_entry(gdt_entry_t *gdt_entry, uint64_t base, uint64_t limit, uint8_t access_byte,
				   uint8_t granularity_byte);
void set_tss_descriptor(int position, uint64_t base, uint64_t limit);
void gdt_init(void);

#endif
