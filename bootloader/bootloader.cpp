#include "../shared/uefi/uefi.h"
#include "../shared/uefi/uefi_console.h"
#include "../shared/uefi/uefi_memory_map.h"
#include "../shared/data_structures/array.h"
#include "../shared/kernel_handover.h"
#include <stddef.h>

#define UP_ARROW_SCANCODE          1
#define DOWN_ARROW_SCANCODE        2
#define ESC_SCANCODE               23
#define DEFAULT_FOREGROUND_COLOR   UEFI_FOREGROUND_BLUE
#define DEFAULT_BACKGROUND_COLOR   UEFI_BACKGROUND_CYAN
#define HIGHLIGHT_FOREGROUND_COLOR UEFI_FOREGROUND_YELLOW
#define HIGHLIGHT_BACKGROUND_COLOR UEFI_BACKGROUND_BLACK

/*******************************************************************************
SET TEXT MODE MENU FUNCTION

Function to provide a menu interface allowing the user to select from available
text modes.
*******************************************************************************/
UEFI_STATUS set_text_mode (UEFI_SYSTEM_TABLE* SystemTable) {

    while (true) {

        // Clear screen to the default background color.
        SystemTable->ConOut->SetAttribute (
            SystemTable->ConOut, 
            UEFI_TEXT_ATTR(DEFAULT_FOREGROUND_COLOR, DEFAULT_BACKGROUND_COLOR)
        );
        SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

        /* Query the selected text mode for num of rows and cols. */
        static int64_t selected_text_mode = 0;
        selected_text_mode = SystemTable->ConOut->Mode->Mode;
        uint64_t max_cols, max_rows;
        int32_t max_num_of_modes = SystemTable->ConOut->Mode->MaxMode;
        SystemTable->ConOut->QueryMode (
            SystemTable->ConOut, 
            selected_text_mode, 
            &max_cols, 
            &max_rows
        );

        // Print verbose information about selected text mode to screen.
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

        // Print row x col information per available text mode to screen.
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

            /* Note: logic only works for less than 10 selected text modes as 
            only 1 key press is buffered.*/
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
                /* Did not get a valid key press corresponding to a number 
                within range or a number. */
                uefi_printf(SystemTable, u"\r\nRecived Scancode : %i ; Char : %s. Invalid text mode.\r\n", k.ScanCode, charbuffer);
            }
        }    
    }

    return UEFI_SUCCESS;
}

/*******************************************************************************
SET GRAPHICS MODE MENU FUNCTION

Function to provide a menu interface allowing the user to select from available
graphic modes.
*******************************************************************************/
UEFI_STATUS set_graphics_mode (UEFI_SYSTEM_TABLE* SystemTable) {

    // Clear screen to the default background color.
    SystemTable->ConOut->SetAttribute (
        SystemTable->ConOut, 
        UEFI_TEXT_ATTR(DEFAULT_FOREGROUND_COLOR, DEFAULT_BACKGROUND_COLOR)
    );
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

    // Locate and return an instance of the Graphics Output Protocol.
    UEFI_GUID gop_guid                 = UEFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    UEFI_GRAPHICS_OUTPUT_PROTOCOL* gop = nullptr;
    UEFI_STATUS gop_status             = SystemTable->BootServices->LocateProtocol(&gop_guid, (void*)nullptr, (void**)&gop);
    if (UEFI_IS_ERROR(gop_status)){
        uefi_printf(SystemTable, u"Could not locate GOP protocol. UEFI Error %i", gop_status);
    }

    // Get maximum number of text rows for the bottom of the menu.
    uint64_t text_max_cols, menu_bottom;
    SystemTable->ConOut->QueryMode(SystemTable->ConOut, SystemTable->ConOut->Mode->Mode, &text_max_cols, &menu_bottom);

    uint64_t mode_info_size;
    uint64_t selected_menu_option                    = 0;
    UEFI_GRAPHICS_OUTPUT_MODE_INFORMATION* mode_info = nullptr;

    while (true) {

        // Clear screen to the default background color.
        SystemTable->ConOut->SetAttribute (
            SystemTable->ConOut, 
            UEFI_TEXT_ATTR(DEFAULT_FOREGROUND_COLOR, DEFAULT_BACKGROUND_COLOR)
        );
        SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

        // Query for information on the current GOP mode.
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
            gop->Blt (
                gop, 
                &blue_bg_pixel, 
                UefiBltVideoFill, 
                0, 0, 0, 0, 
                mode_info->HorizontalResolution, 
                mode_info->VerticalResolution, 
                0
            );
        }
    }

    return UEFI_SUCCESS;
}

/*******************************************************************************
PRINT DATE TIME FUNCTION

Function to print date & time on screen when the corresponding event is 
triggered.
*******************************************************************************/
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

/*******************************************************************************
GET ESP ROOT FUNCTION

Returns a pointer to the file handle of the root directory directory of the EFI 
System Partition (ESP).
*******************************************************************************/
UEFI_FILE_PROTOCOL* get_esp_root (UEFI_HANDLE ImageHandle, UEFI_SYSTEM_TABLE* SystemTable) {

    // Open the loaded image protocol for this UEFI image. This will produce 
    // the device handle of the device that the image was loaded from.
    UEFI_GUID loaded_image_protocol_guid = UEFI_LOADED_IMAGE_PROTOCOL_GUID;
    UEFI_LOADED_IMAGE_PROTOCOL* lip;
    UEFI_STATUS status = SystemTable->BootServices->OpenProtocol (
        ImageHandle, 
        &loaded_image_protocol_guid, 
        (void**)&lip, ImageHandle, 
        nullptr, 
        UEFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL
    );

    if (UEFI_IS_ERROR(status)){
        uefi_printf(SystemTable, u"Loaded Image Protocol not found on handle. UEFI Error %i", status);
    }

    // Query the device handle for the Simple Filesystem protocol in order to 
    // access the root of the ESP.
    UEFI_GUID simple_file_system_guid = UEFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    UEFI_SIMPLE_FILE_SYSTEM_PROTOCOL* sfs;
    status = SystemTable->BootServices->OpenProtocol (
        lip->DeviceHandle, 
        &simple_file_system_guid, 
        (void**)&sfs,
        ImageHandle,
        nullptr,
        UEFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

    if (UEFI_IS_ERROR(status)){
        uefi_printf(SystemTable, u"Simple File System Protocol not found on handle. UEFI Error %i", status);
    }

    // Obtain a pointer to the file handle of the root directory of the ESP.
    UEFI_FILE_PROTOCOL* fp;
    status = sfs->OpenVolume(sfs, &fp);

    if (UEFI_IS_ERROR(status)){
        uefi_printf(SystemTable, u"Simple File System Protocol could not open volume. UEFI Error %i", status);
    }

    // Return the pointer.
    return fp;
}

/*******************************************************************************
READ ESP FILE INTO BUFFER FUNCTION

Returns a pointer to the file handle of the root directory directory of the EFI 
System Partition (ESP).
*******************************************************************************/
void* read_esp_file_into_buffer (
    UEFI_HANDLE        ImageHandle,
    UEFI_SYSTEM_TABLE* SystemTable, 
    char16_t*          path, 
    uint64_t*          buffer_size
) {

    UEFI_FILE_PROTOCOL* root = get_esp_root (ImageHandle, SystemTable);

    // Get a pointer to the file handle of the file specified in the path parameter.
    UEFI_FILE_PROTOCOL* fp;
    root->Open(root, &fp, path, UEFI_FILE_MODE_READ, 0);
    root->Close(root);

    // Get metadata about the file to obtain the size of the file.
    UEFI_FILE_INFO fp_info;
    uint64_t fp_info_size = sizeof(fp_info);
    UEFI_GUID file_info_guid = UEFI_FILE_INFO_ID;        
    fp->GetInfo(fp, &file_info_guid, &fp_info_size, &fp_info); 

    // Allocate a pool of memory of size of the file. 
    void* file_buffer = nullptr;
    uint64_t file_buffer_size = fp_info.FileSize;
    SystemTable->BootServices->AllocatePool(UefiLoaderData, file_buffer_size, &file_buffer);

    // Read file into buffer.
    fp->Read(fp, &file_buffer_size, file_buffer);

    // Return the buffer and the size of the buffer.
    *buffer_size = file_buffer_size;
    return file_buffer;

}

/*******************************************************************************
PRINT CONTENTS OF A FILE BUFFER ONTO SCREEN FUNCTION
*******************************************************************************/
UEFI_STATUS print_file_buffer_contents (UEFI_SYSTEM_TABLE* SystemTable, void* file_contents_buffer, uint64_t file_contents_buffer_size) {
    
    // Readout the file contents byte at a time.
    char* read_char = (char *) file_contents_buffer;
    for (uint64_t idx = file_contents_buffer_size; idx > 0; idx--){
        char16_t s[2];
        s[1] = '\0';
        s[0] = (char16_t)*read_char;
        uefi_printf(SystemTable, u"%s", s);
        read_char++;
    }
}

/*******************************************************************************
READ ESP PARTITION CONTENTS MENU FUNCTION
*******************************************************************************/
UEFI_STATUS read_esp (UEFI_HANDLE ImageHandle, UEFI_SYSTEM_TABLE* SystemTable) {

    // Get a pointer to the file handle of the root directory directory of the ESP.
    UEFI_FILE_PROTOCOL* fp = get_esp_root (ImageHandle, SystemTable);

    uint64_t selected_menu_option = 0;

    while (true) {

        // Clear screen to the default background color.
        SystemTable->ConOut->SetAttribute (
            SystemTable->ConOut, 
            UEFI_TEXT_ATTR(DEFAULT_FOREGROUND_COLOR, DEFAULT_BACKGROUND_COLOR)
        );
        SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

        uint64_t file_count = 0;

        UEFI_FILE_INFO file_info;
        UEFI_FILE_INFO selected_file_info;
        uint64_t file_info_size = sizeof(file_info);
        fp->SetPosition(fp, 0); // Reset the process of reading the directory entries.
        
        // Reads directory entry.
        fp->Read(fp, &file_info_size, (void*)&file_info);

        // Read all directory entries.
        while (file_info_size > 0) {

            if (file_count == selected_menu_option) {
                SystemTable->ConOut->SetAttribute(SystemTable->ConOut, UEFI_TEXT_ATTR(HIGHLIGHT_FOREGROUND_COLOR, HIGHLIGHT_BACKGROUND_COLOR));
                selected_file_info = file_info;
            }

            // Print file name and whether it is a directory or file on screen.
            uefi_printf (
                SystemTable, u"[%s] %s\r\n", 
                (file_info.Attribute & UEFI_FILE_DIRECTORY) ? u"DIR" : u"FILE", 
                file_info.FileName);

            if (file_count == selected_menu_option) {
                SystemTable->ConOut->SetAttribute(SystemTable->ConOut, UEFI_TEXT_ATTR(DEFAULT_FOREGROUND_COLOR, DEFAULT_BACKGROUND_COLOR));
            }

            // Read next entry if any.
            file_info_size = sizeof(file_info);
            fp->Read(fp, &file_info_size, (void*)&file_info);
            
            // Increment file count.
            file_count++;
        }

        // Waiting for a keystroke. 
        UEFI_INPUT_KEY k = uefi_wait_for_keystroke(SystemTable);

        if (k.ScanCode == ESC_SCANCODE) { // Escape key pressed.
            return UEFI_SUCCESS;
        } else if (k.ScanCode == UP_ARROW_SCANCODE) {
            // Move selected menu option index down in value as user scrolls up. Rollover
            // index to the maximum if index is at the minimum.
            if (selected_menu_option == 0) {
                selected_menu_option = (file_count - 1);
            } else {
                selected_menu_option--;
            }
        } else if (k.ScanCode == DOWN_ARROW_SCANCODE) { // Down key pressed.
            // Move selected menu option index up in value as user scrolls down. Rollover
            // index to the minimum if index is at the maximum.
            if ((selected_menu_option + 1) == file_count) {
                selected_menu_option = 0;
            } else {
                selected_menu_option++;
            }
        } else if (k.UnicodeChar == u'\r') { // Enter Key pressed.
            if ((selected_file_info.Attribute & UEFI_FILE_DIRECTORY)) // Is the selection a directory?
            {

                // Close the existing file handle for the current location and
                // assign the new handle for the new location.
                UEFI_FILE_PROTOCOL* new_fp;
                fp->Open(fp, &new_fp, selected_file_info.FileName, UEFI_FILE_MODE_READ, 0);
                fp->Close(fp);
                fp = new_fp;

            } else {

                // Dynamically allocate memory for a buffer large enough to store the file.
                void* file_contents_buffer;
                uint64_t file_contents_buffer_size = selected_file_info.FileSize;
                UEFI_STATUS status = SystemTable->BootServices->AllocatePool(UefiLoaderData, file_contents_buffer_size, &file_contents_buffer);

                if (UEFI_IS_ERROR(status)){
                    uefi_printf(SystemTable, u"Could not allocate pool of memory for buffer. UEFI Error %i", status);
                }

                // Open a file handle for the file.
                UEFI_FILE_PROTOCOL* file_to_read;
                status = fp->Open(fp, &file_to_read, selected_file_info.FileName, UEFI_FILE_MODE_READ, 0);

                if (UEFI_IS_ERROR(status)){
                    uefi_printf(SystemTable, u"Could not open a file handle. UEFI Error %i", status);
                }

                // Read the opened file's contents into the buffer.
                status = file_to_read->Read(file_to_read, &file_contents_buffer_size, file_contents_buffer);

                if (UEFI_IS_ERROR(status)){
                    uefi_printf(SystemTable, u"Could not read file contents into buffer. UEFI Error %i", status);
                }

                // Readout the file contents.
                print_file_buffer_contents (SystemTable, file_contents_buffer, file_contents_buffer_size);

                // Free the pool of memory allocated.
                SystemTable->BootServices->FreePool(file_contents_buffer);

                // Wait for keypress to continue.
                uefi_printf(SystemTable, u"Press any key to continue...");
                uefi_wait_for_keystroke(SystemTable);

            }
        }
    }

    return UEFI_SUCCESS;
}

/*******************************************************************************
LAUNCH COSMOS FUNCTION
*******************************************************************************/
UEFI_STATUS UEFI_API launch_cosmOS (UEFI_HANDLE ImageHandle, UEFI_SYSTEM_TABLE* SystemTable, UEFI_EVENT datetime_event) {

    // Clear screen to the default background color.
    SystemTable->ConOut->SetAttribute (
        SystemTable->ConOut, 
        UEFI_TEXT_ATTR(DEFAULT_FOREGROUND_COLOR, DEFAULT_BACKGROUND_COLOR)
    );
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

    // Kill the date and time event before loading kernel.
    SystemTable->BootServices->CloseEvent(datetime_event);

    // Locate and return an instance of the Graphics Output Protocol.
    UEFI_GUID gop_guid                 = UEFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    UEFI_GRAPHICS_OUTPUT_PROTOCOL* gop = nullptr;
    UEFI_STATUS status                 = SystemTable->BootServices->LocateProtocol(&gop_guid, (void*)nullptr, (void**)&gop);
    if (UEFI_IS_ERROR(status)){
        uefi_printf(SystemTable, u"Could not locate GOP protocol. UEFI Error %i", status);
    }

    /* Populate kernel handover parameters object with memory map and GOP mode
    for access to framebuffer.*/
    Kernel_Handover* k;
    k->gop = *gop->Mode;
    uefi_get_memory_map (SystemTable, &k->memory_map);

    // Exit UEFI boot services.
    SystemTable->BootServices->ExitBootServices(ImageHandle, k->memory_map.key);

    /* Get a pointer to the file handle of the root directory of the ESP. */
    UEFI_FILE_PROTOCOL* fp = get_esp_root (ImageHandle, SystemTable);

    // Read the kernel binary executable into a buffer.
    uint64_t file_buffer_size;
    void* kernel_buffer = read_esp_file_into_buffer (ImageHandle, SystemTable, u"\\EFI\\BOOT\\kernel.bin", &file_buffer_size);

    /* Establish a function pointer pointing to the kinary binary's executable
    code. */
    void UEFI_API (*entry_point)(Kernel_Handover*) = NULL;
    *(void **)&entry_point = kernel_buffer; 

    /* Call the kernel's entry point to turn control over to the operating 
    system. */
    entry_point(k);

    // Should never reach this point in execution.
    __builtin_unreachable();

    return UEFI_SUCCESS;
}

/*******************************************************************************
MAIN FUNCTION
*******************************************************************************/
extern "C" { // Avoids name mangling of the UEFI entry point.
UEFI_STATUS UEFI_API uefi_main (UEFI_HANDLE ImageHandle, UEFI_SYSTEM_TABLE* SystemTable) 
{

    /* Reset Console Device, clear screen to background color, and set cursor
    to (0,0) */
    SystemTable->ConOut->Reset(SystemTable->ConOut, false);
    
    // Clear screen to the default background color.
    SystemTable->ConOut->SetAttribute (
        SystemTable->ConOut, 
        UEFI_TEXT_ATTR(DEFAULT_FOREGROUND_COLOR, DEFAULT_BACKGROUND_COLOR)
    );
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

    // Create an event that is of TIMER and NOTIFY SIGNAL type that will
    // queue the print_datetime function when the event is signaled. Pass
    // the SystemTable as a parameter to the callback function print_datetime.
    UEFI_EVENT datetime_event;
    SystemTable->BootServices->CreateEvent((EVT_TIMER | EVT_NOTIFY_SIGNAL), TPL_CALLBACK, print_datetime, (void*)SystemTable, &datetime_event);

    /* Create a timer that will signal the event every 10000000 100ns units or 
    1 second. */
    SystemTable->BootServices->SetTimer(datetime_event, TimerPeriodic, 10000000);

    const char16_t* menu_options[] = {
        u"Set Text Mode",
        u"Set Graphics Mode",
        u"Read ESP",
        u"Launch cosmOS!"
    };

    uint64_t selected_menu_option = 0;

    while(true) {

        // Clear screen to the default background color.
        SystemTable->ConOut->SetAttribute (
            SystemTable->ConOut, 
            UEFI_TEXT_ATTR(DEFAULT_FOREGROUND_COLOR, DEFAULT_BACKGROUND_COLOR)
        );
        SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

        /* Reprint menu options and highlight the selected menu option. */
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

        if (k.ScanCode == ESC_SCANCODE) { // Escape key pressed.
            // Shutdown system.
            SystemTable->RuntimeServices->ResetSystem(UefiResetShutdown, UEFI_SUCCESS, 0, NULL);
        } else if (k.ScanCode == UP_ARROW_SCANCODE) { // Up arrow key pressed.
            // Move selected menu option index down in value as user scrolls up. Rollover
            // index to the maximum if index is at the minimum.
            if (selected_menu_option == 0) {
                selected_menu_option = (ARRAY_SIZE(menu_options) - 1);
            } else {
                selected_menu_option--;
            }
        } else if (k.ScanCode == DOWN_ARROW_SCANCODE) { // Down arrow key pressed.
            // Move selected menu option index up in value as user scrolls down. Rollover
            // index to the minimum if index is at the maximum.
            if ((selected_menu_option + 1) == ARRAY_SIZE(menu_options)) {
                selected_menu_option = 0;
            } else {
                selected_menu_option++;
            }
        } else if (k.UnicodeChar == u'\r') { // Enter Key pressed.
            // Call appropriate function depending on selected option.
            if (selected_menu_option == 0) {
                set_text_mode (SystemTable);
            } else if (selected_menu_option == 1) {
                set_graphics_mode (SystemTable);
            } else if (selected_menu_option == 2) {
                read_esp (ImageHandle, SystemTable);
            } else if (selected_menu_option == 3) {
                launch_cosmOS(ImageHandle, SystemTable, datetime_event);
            }
        }
    }

    return UEFI_SUCCESS;
    
}}
