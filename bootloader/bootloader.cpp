#include "../shared/uefi/uefi.h"
#include "../shared/uefi/uefi_console.h"
#include "../shared/data_structures/array.h"
#include <stddef.h>

#define UP_ARROW_SCANCODE          1
#define DOWN_ARROW_SCANCODE        2
#define ESC_SCANCODE               23
#define DEFAULT_FOREGROUND_COLOR   UEFI_FOREGROUND_BLUE
#define DEFAULT_BACKGROUND_COLOR   UEFI_BACKGROUND_CYAN
#define HIGHLIGHT_FOREGROUND_COLOR UEFI_FOREGROUND_YELLOW
#define HIGHLIGHT_BACKGROUND_COLOR UEFI_BACKGROUND_BLACK

UEFI_STATUS set_text_mode (UEFI_SYSTEM_TABLE* SystemTable) {

    /*Set background and foreground colors.*/
    SystemTable->ConOut->SetAttribute(SystemTable->ConOut, UEFI_TEXT_ATTR(DEFAULT_FOREGROUND_COLOR, DEFAULT_BACKGROUND_COLOR));

    while (true) {

        /*Clear screen to background color, and set cursor to (0,0)*/
        SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

        static int64_t selected_text_mode = 0;
        selected_text_mode = SystemTable->ConOut->Mode->Mode;

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
                    SystemTable->ConOut->Mode->Mode,
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

        char16_t charbuffer[2] = {'\0', '\0'};
        while (true) {

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
                return UEFI_SUCCESS;
            } else {
                // Did not get a valid key press corresponding to a number within range or a number.
                uefi_printf(SystemTable, u"\r\nRecived Scancode : %i ; Char : %s. Invalid text mode.\r\n", k.ScanCode, charbuffer);
            }
        }    
    }

    return UEFI_SUCCESS;
}

UEFI_STATUS set_graphics_mode (UEFI_SYSTEM_TABLE* SystemTable) {

    /*Set background and foreground colors.*/
    SystemTable->ConOut->SetAttribute(SystemTable->ConOut, UEFI_TEXT_ATTR(DEFAULT_FOREGROUND_COLOR, DEFAULT_BACKGROUND_COLOR));

    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

    UEFI_GUID gop_guid = UEFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    UEFI_GRAPHICS_OUTPUT_PROTOCOL* gop = nullptr;

    UEFI_STATUS gop_status = SystemTable->BootServices->LocateProtocol(&gop_guid, (void*)nullptr, (void**)&gop);

    if (UEFI_IS_ERROR(gop_status)){
        uefi_printf(SystemTable, u"Could not locate GOP protocol. UEFI Error %i", gop_status);
    }

    uint64_t mode_info_size;
    UEFI_GRAPHICS_OUTPUT_MODE_INFORMATION* mode_info = nullptr;

    // Get maximum number of text rows for the bottom of the menu.
    uint64_t text_max_cols, menu_bottom;
    SystemTable->ConOut->QueryMode(SystemTable->ConOut, SystemTable->ConOut->Mode->Mode, &text_max_cols, &menu_bottom);

    uint64_t selected_menu_option = 0;

    while (true) {

        /*Clear screen to background color, and set cursor to (0,0)*/
        SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

        gop_status = gop->QueryMode(gop, gop->Mode->Mode, &mode_info_size, &mode_info);

        if (UEFI_IS_ERROR(gop_status)){
            uefi_printf(SystemTable, u"Could not query GOP mode. UEFI Error %i", gop_status);
        }

        uefi_printf(SystemTable, u"Max Mode: %i\r\n"
                                 u"Current Mode: %i\r\n"
                                 u"WidthxHeight: %ix%i\r\n"
                                 u"Framebuffer Address: %u\r\n"
                                 u"Framebuffer Size: %i\r\n"
                                 u"Pixel Format: %i\r\n"
                                 u"Pixels Per Scan Line: %i\r\n",
                                 gop->Mode->MaxMode,
                                 gop->Mode->Mode,
                                 mode_info->HorizontalResolution, mode_info->VerticalResolution,
                                 gop->Mode->FrameBufferBase,
                                 gop->Mode->FrameBufferSize,
                                 mode_info->PixelFormat,
                                 mode_info->PixelsPerScanLine
        );

        uefi_printf(SystemTable, u"Available graphics modes:\r\n");

        // Current cursor row after above information prints.
        int32_t  menu_top = SystemTable->ConOut->Mode->CursorRow;

        // Length of menu so that above information remains on screen.
        uint64_t menu_len = (menu_bottom - 1) - menu_top;

        // Print only a select amount s.t. all options are viewable on screen
        // alongside above information.
        for (uint64_t idx = selected_menu_option; idx < (selected_menu_option + menu_len); idx++) {
            if (idx == selected_menu_option) {
                SystemTable->ConOut->SetAttribute(SystemTable->ConOut, UEFI_TEXT_ATTR(HIGHLIGHT_FOREGROUND_COLOR, HIGHLIGHT_BACKGROUND_COLOR));
            }
            gop->QueryMode(gop, idx, &mode_info_size, &mode_info);
            uefi_printf(SystemTable, u"GOP Mode # %i (h x v); %ix%i\r\n", idx, mode_info->HorizontalResolution, mode_info->VerticalResolution);
            if (idx == selected_menu_option) {
                SystemTable->ConOut->SetAttribute(SystemTable->ConOut, UEFI_TEXT_ATTR(DEFAULT_FOREGROUND_COLOR, DEFAULT_BACKGROUND_COLOR));
            }
            if ((idx + 1) > (gop->Mode->MaxMode - 1)) {
                break;
            }
        }

        // Waiting for a keystroke. 
        UEFI_INPUT_KEY k = uefi_wait_for_keystroke(SystemTable);

        if (k.ScanCode == ESC_SCANCODE) {
            return UEFI_SUCCESS;
        } else if (k.ScanCode == UP_ARROW_SCANCODE) {
            if (selected_menu_option == 0) {
                selected_menu_option = (gop->Mode->MaxMode - 1);
            } else {
                selected_menu_option--;
            }
        } else if (k.ScanCode == DOWN_ARROW_SCANCODE) {
            if ((selected_menu_option + 1) == gop->Mode->MaxMode) {
                selected_menu_option = 0;
            } else {
                selected_menu_option++;
            }
        } else if (k.UnicodeChar == u'\r') { // Enter Key pressed.
            gop->SetMode(gop, selected_menu_option);
            gop->QueryMode(gop, selected_menu_option, &mode_info_size, &mode_info);
            UEFI_GRAPHICS_OUTPUT_BLT_PIXEL blue_bg_pixel = {0xFF, 0x00, 0x00, 0x00};
            // Fill the screen with blue after changing resolution.
            gop->Blt (gop, &blue_bg_pixel, UefiBltVideoFill, 0, 0, 0, 0, mode_info->HorizontalResolution, mode_info->VerticalResolution, 0);
        }
    }

    return UEFI_SUCCESS;
}

void UEFI_API print_datetime (UEFI_EVENT Event, void* Context) {

    UEFI_SYSTEM_TABLE* SystemTable = (UEFI_SYSTEM_TABLE *) Context;

    // Save current text cursor row and col.
    int32_t saved_text_cursor_row = SystemTable->ConOut->Mode->CursorRow;
    int32_t saved_text_cursor_col = SystemTable->ConOut->Mode->CursorColumn;

    uint64_t text_max_cols, text_max_rows;
    SystemTable->ConOut->QueryMode(SystemTable->ConOut, SystemTable->ConOut->Mode->Mode, &text_max_cols, &text_max_rows);

    // Date time will print out MM/DD/YYYY HH:MM:SS which is 19 characters.
    // Set cursor 19 characters to the right of the last column of row 0.
    // This will set the date time to printed on the top right of the screen.
    SystemTable->ConOut->SetCursorPosition(SystemTable->ConOut, ((text_max_cols - 1) - 19), 0);

    // Get current date and time from battery powered real time clock on platform.
    UEFI_TIME datetime;
    SystemTable->RuntimeServices->GetTime(&datetime, nullptr);

    // Print date time in format MM/DD/YYYY HH:MM:SS.
    uefi_printf(SystemTable, u"%u/%u/%u %u:%u:%u", 
    datetime.Month, datetime.Day, datetime.Year, 
    datetime.Hour, datetime.Minute, datetime.Second);

    // Restore text cursor row and col.
    SystemTable->ConOut->SetCursorPosition(SystemTable->ConOut, saved_text_cursor_col, saved_text_cursor_row);
}

extern "C" { // Avoids name mangling of the UEFI entry point.
UEFI_STATUS UEFI_API uefi_main (UEFI_HANDLE ImageHandle, UEFI_SYSTEM_TABLE* SystemTable) {

    /* Reset Console Device, clear screen to background color, and set cursor
    to (0,0) */
    SystemTable->ConOut->Reset(SystemTable->ConOut, false);
    
    /*Set background and foreground colors.*/
    SystemTable->ConOut->SetAttribute(SystemTable->ConOut, UEFI_TEXT_ATTR(DEFAULT_FOREGROUND_COLOR, DEFAULT_BACKGROUND_COLOR));

    UEFI_EVENT datetime_event;

    // Create an event that is of TIMER and NOTIFY SIGNAL type that will
    // queue the print_datetime function when the event is signaled. Pass
    // the SystemTable as a parameter to the callback function print_datetime.
    SystemTable->BootServices->CreateEvent((EVT_TIMER | EVT_NOTIFY_SIGNAL), TPL_CALLBACK, print_datetime, (void*)SystemTable, &datetime_event);

    // Create a timer that will signal the event every 10000000 100ns units or 1 second.
    SystemTable->BootServices->SetTimer(datetime_event, TimerPeriodic, 10000000);

    const char16_t* menu_options[] = {
        u"Set Text Mode",
        u"Set Graphics Mode"
    };

    uint64_t selected_menu_option = 0;

    while(true) {

        SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

        // Print menu options and highlight the selected menu option.
        for (uint64_t idx = 0; idx < ARRAY_SIZE(menu_options); idx++) {
            if (idx == selected_menu_option) {
                SystemTable->ConOut->SetAttribute(SystemTable->ConOut, UEFI_TEXT_ATTR(HIGHLIGHT_FOREGROUND_COLOR, HIGHLIGHT_BACKGROUND_COLOR));
            }
            uefi_printf(SystemTable, u"%s\r\n", menu_options[idx]);
            if (idx == selected_menu_option) {
                SystemTable->ConOut->SetAttribute(SystemTable->ConOut, UEFI_TEXT_ATTR(DEFAULT_FOREGROUND_COLOR, DEFAULT_BACKGROUND_COLOR));
            }
        }

        // Waiting for a keystroke. 
        UEFI_INPUT_KEY k = uefi_wait_for_keystroke(SystemTable);

        if (k.ScanCode == ESC_SCANCODE) {
            SystemTable->RuntimeServices->ResetSystem(UefiResetShutdown, UEFI_SUCCESS, 0, NULL);
        } else if (k.ScanCode == UP_ARROW_SCANCODE) {
            if (selected_menu_option == 0) {
                selected_menu_option = (ARRAY_SIZE(menu_options) - 1);
            } else {
                selected_menu_option--;
            }
        } else if (k.ScanCode == DOWN_ARROW_SCANCODE) {
            if ((selected_menu_option + 1) == ARRAY_SIZE(menu_options)) {
                selected_menu_option = 0;
            } else {
                selected_menu_option++;
            }
        } else if (k.UnicodeChar == u'\r') { // Enter Key pressed.
            if (selected_menu_option == 0) {
                set_text_mode (SystemTable);
            } else if (selected_menu_option == 1) {
                set_graphics_mode (SystemTable);
            }
        }
    }

    return UEFI_SUCCESS;

}}
