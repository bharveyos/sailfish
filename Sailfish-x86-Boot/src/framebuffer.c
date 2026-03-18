#include <efi.h>
#include <efilib.h>

EFI_STATUS init_framebuffer(EFI_GRAPHICS_OUTPUT_PROTOCOL **graphicsOutput)
{
	EFI_STATUS					  status;
	EFI_GUID					  graphicsOutputGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	EFI_GRAPHICS_OUTPUT_PROTOCOL *graphicsOutputProtocol;

	status = uefi_call_wrapper(BS->LocateProtocol, 3, &graphicsOutputGuid, NULL, (VOID **)&graphicsOutputProtocol);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to locate Graphics Output Protocol: %r\n", status);
		return status;
	}

	*graphicsOutput = graphicsOutputProtocol;

	return EFI_SUCCESS;
}

void print_framebuffer_info(EFI_GRAPHICS_OUTPUT_PROTOCOL *graphicsOutput)
{
	Print(L"Frame buffer initialized at: %p\n", graphicsOutput->Mode->FrameBufferBase);
	Print(L"Frame buffer size: %d\n", graphicsOutput->Mode->FrameBufferSize);
	Print(L"Frame buffer width: %d\n", graphicsOutput->Mode->Info->HorizontalResolution);
	Print(L"Frame buffer height: %d\n", graphicsOutput->Mode->Info->VerticalResolution);
	Print(L"Frame buffer pixel format: %d\n", graphicsOutput->Mode->Info->PixelFormat);
	Print(L"Frame buffer pixels per scanline: %d\n", graphicsOutput->Mode->Info->PixelsPerScanLine);
}