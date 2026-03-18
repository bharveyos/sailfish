#include <efi.h>
#include <efilib.h>

VOID *get_acpi_table()
{
	for (size_t i = 0; i < ST->NumberOfTableEntries; ++i)
	{
		EFI_CONFIGURATION_TABLE *table = &ST->ConfigurationTable[i];
		if (CompareGuid(&table->VendorGuid, &AcpiTableGuid))
		{
			Print(L"RSDP found at %p\n", table->VendorTable);
			return (void *)table->VendorTable;
		}
	}
	return NULL;
}