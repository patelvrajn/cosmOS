#include "../shared/uefi/uefi.h"
#include "../shared/uefi/uefi_console.h"
#include <stddef.h>

extern "C" { // Avoids name mangling of the UEFI entry point.
UEFI_STATUS UEFI_API uefi_main (UEFI_HANDLE ImageHandle, UEFI_SYSTEM_TABLE* SystemTable) {

    /* Reset Console Device, clear screen to background color, and set cursor
    to (0,0) */
    SystemTable->ConOut->Reset(SystemTable->ConOut, false);

    /*Set background and foreground colors.*/
    SystemTable->ConOut->SetAttribute(SystemTable->ConOut, UEFI_TEXT_ATTR(UEFI_FOREGROUND_BLUE, UEFI_BACKGROUND_CYAN));

    bool running = true;
    while (running) {

        /*Clear screen to background color, and set cursor to (0,0)*/
        SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

        static int64_t selected_text_mode = 0;
        selected_text_mode = SystemTable->ConOut->Mode->CurrentMode;

        /*Query the selected text mode for num of rows and cols.*/
        uint64_t max_cols, max_rows;
        int32_t max_num_of_modes = SystemTable->ConOut->Mode->MaxMode;
        SystemTable->ConOut->QueryMode(SystemTable->ConOut, selected_text_mode, &max_cols, &max_rows);

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
                    max_num_of_modes,
                    SystemTable->ConOut->Mode->CurrentMode,
                    SystemTable->ConOut->Mode->Attribute,
                    SystemTable->ConOut->Mode->CursorRow,
                    SystemTable->ConOut->Mode->CursorColumn,
                    SystemTable->ConOut->Mode->CursorVisible,
                    max_cols,
                    max_rows);

        for (uint64_t idx = 0; idx < (uint64_t)max_num_of_modes; idx++) {

            SystemTable->ConOut->QueryMode(SystemTable->ConOut, idx, &max_cols, &max_rows);

            uefi_printf(SystemTable, u"Text Mode (rows x cols) %i: %ix%i\r\n", idx, max_rows, max_cols);

        }

        static const uint16_t ESC_SCANCODE = 23;
        char16_t charbuffer[2] = {'\0', '\0'};
        while (1) {

            // Wait for user to press a key choosing a valid text mode.
            uefi_printf(SystemTable, u"Select text mode (0-%i)", (max_num_of_modes - 1));
            UEFI_INPUT_KEY k = uefi_wait_for_keystroke(SystemTable);

            // Store character of key pressed, if any.
            charbuffer[0] = k.UnicodeChar;

            // Note: logic only works for less than 10 selected text modes as only 1 key press is buffered.
            selected_text_mode = k.UnicodeChar - u'0';
            if (selected_text_mode >= 0 && selected_text_mode < max_num_of_modes) {
                
                // Set text mode to mode requested by user. 
                UEFI_STATUS status = SystemTable->ConOut->SetMode(SystemTable->ConOut, selected_text_mode);

                // Check for error.
                if(UEFI_IS_ERROR(status)) {
                    uefi_printf(SystemTable, u"\r\nUEFI ERROR %i while selecting text mode.\r\n", status);    
                } else {
                    break; // Successfully set new text mode, redraw screen.
                }

            } else if (k.ScanCode == ESC_SCANCODE) {
                SystemTable->RuntimeServices->ResetSystem(UefiResetShutdown, UEFI_SUCCESS, 0, NULL);
            } else {
                // Did not get a valid key press corresponding to a number within range or a number.
                uefi_printf(SystemTable, u"\r\nRecived Scancode : %i ; Char : %s. Invalid text mode.\r\n", k.ScanCode, charbuffer);
            }
        }    
    }
    
    return UEFI_SUCCESS;

}}
