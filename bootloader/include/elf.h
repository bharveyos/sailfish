#ifndef ELF_H
#define ELF_H

#include <efi.h>
#include <efilib.h>

#define EI_NIDENT (16)
#define ELFCLASS64 2

#define EI_DATA 5	  /* Data encoding byte index */
#define ELFDATANONE 0 /* Invalid data encoding */
#define ELFDATA2LSB 1 /* 2's complement, little endian */
#define ELFDATA2MSB 2 /* 2's complement, big endian */
#define ELFDATANUM 3

#define PT_LOAD 1

EFI_STATUS read_elf_file(EFI_FILE *elf_file, VOID **elf_header_buffer, VOID **elf_program_header_buffer);
EFI_STATUS read_elf_identity(EFI_FILE *elf_file, UINT8 **elf_identity_buffer);
EFI_STATUS validate_elf(UINT8 *elf_identity_buffer);

typedef struct
{
	unsigned char e_ident[EI_NIDENT];
	UINT16		  e_type;
	UINT16		  e_machine;
	UINT32		  e_version;
	UINT64		  e_entry;
	UINT64		  e_phoff;
	UINT64		  e_shoff;
	UINT32		  e_flags;
	UINT16		  e_ehsize;
	UINT16		  e_phentsize;
	UINT16		  e_phnum;
	UINT16		  e_shentsize;
	UINT16		  e_shnum;
	UINT16		  e_shstrndx;
} Elf64_Ehdr;

typedef struct
{
	UINT32 p_type;
	UINT32 p_flags;
	UINT64 p_offset;
	UINT64 p_vaddr;
	UINT64 p_paddr;
	UINT64 p_filesz;
	UINT64 p_memsz;
	UINT64 p_align;
} Elf64_Phdr;

#endif