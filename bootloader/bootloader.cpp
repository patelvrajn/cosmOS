#include "../shared/uefi/uefi.h"

extern "C" {
UEFI_STATUS UEFI_API uefi_main (UEFI_HANDLE ImageHandle, UEFI_SYSTEM_TABLE* SystemTable) {

    /* Reset Console Device, clear screen to background color, and set cursor
    to (0,0) */
    SystemTable->ConOut->Reset(SystemTable->ConOut, false);

    /*Set background and foreground colors.*/
    SystemTable->ConOut->SetAttribute(SystemTable->ConOut, UEFI_TEXT_ATTR(UEFI_FOREGROUND_BLUE, UEFI_BACKGROUND_CYAN));

    /*Print string.*/
    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"Hello World!");

    while (1);

    return UEFI_SUCCESS;

}}
