#include "../shared/uefi/uefi.h"
#include "../shared/uefi/uefi_console.h"

extern "C" {
UEFI_STATUS UEFI_API uefi_main (UEFI_HANDLE ImageHandle, UEFI_SYSTEM_TABLE* SystemTable) {

    /* Reset Console Device, clear screen to background color, and set cursor
    to (0,0) */
    SystemTable->ConOut->Reset(SystemTable->ConOut, false);

    /*Set background and foreground colors.*/
    SystemTable->ConOut->SetAttribute(SystemTable->ConOut, UEFI_TEXT_ATTR(UEFI_FOREGROUND_BLUE, UEFI_BACKGROUND_CYAN));

    /*Clear screen to background color, and set cursor to (0,0)*/
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

    uint64_t max_cols, max_rows;

    SystemTable->ConOut->QueryMode(SystemTable->ConOut, SystemTable->ConOut->Mode->CurrentMode, &max_cols, &max_rows);

    /*Print string.*/
    uefi_printf(SystemTable, u"Current Text Mode:\r\n"
                u"Max Mode: %i\r\n"
                u"Current Mode: %i\r\n"
                u"Attribute: %i\r\n"
                u"Cursor Row: %i\r\n"
                u"Cursor Column: %i\r\n"
                u"Cursor Visible: %i\r\n"
                u"Columns: %i\r\n"
                u"Rows: %i\r\n",
                SystemTable->ConOut->Mode->MaxMode,
                SystemTable->ConOut->Mode->CurrentMode,
                SystemTable->ConOut->Mode->Attribute,
                SystemTable->ConOut->Mode->CursorRow,
                SystemTable->ConOut->Mode->CursorColumn,
                SystemTable->ConOut->Mode->CursorVisible,
                max_cols,
                max_rows);

    while (1);

    return UEFI_SUCCESS;

}}
