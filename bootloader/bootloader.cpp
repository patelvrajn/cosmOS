#include "../shared/uefi/uefi.h"
#include "../shared/uefi/uefi_console.h"
#include "../shared/uefi/uefi_memory_map.h"
#include "../shared/uefi/uefi_file_io.h"
#include "../shared/data_structures/array.h"
#include "../shared/kernel_handover.h"
#include "../shared/assembly_wrappers/registers.h"
#include "../shared/graphics/fonts/pc_screen_font_v1_renderer.h"
#include "../shared/memory/physical_memory_manager.h"
#include <stddef.h>

#define UP_ARROW_SCANCODE          1
#define DOWN_ARROW_SCANCODE        2
#define ESC_SCANCODE               23
#define DEFAULT_FOREGROUND_COLOR   UEFI_FOREGROUND_BLUE
#define DEFAULT_BACKGROUND_COLOR   UEFI_BACKGROUND_CYAN
#define HIGHLIGHT_FOREGROUND_COLOR UEFI_FOREGROUND_YELLOW
#define HIGHLIGHT_BACKGROUND_COLOR UEFI_BACKGROUND_BLACK

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
READ ESP FILE INTO BUFFER FUNCTION

Returns a pointer to a buffer containing the contents of a file in the EFI 
System Partition (ESP) and the size of the buffer.
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

                UEFI_PRINT_ERROR(SystemTable, status, "Could not allocate pool of memory for buffer");

                // Open a file handle for the file.
                UEFI_FILE_PROTOCOL* file_to_read;
                status = fp->Open(fp, &file_to_read, selected_file_info.FileName, UEFI_FILE_MODE_READ, 0);

                UEFI_PRINT_ERROR(SystemTable, status, "Could not open a file handle for selected file");

                // Read the opened file's contents into the buffer.
                status = file_to_read->Read(file_to_read, &file_contents_buffer_size, file_contents_buffer);

                UEFI_PRINT_ERROR(SystemTable, status, "Could not read file contents into buffer for selected file");

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

UEFI_STATUS display_memory_map (UEFI_HANDLE ImageHandle, UEFI_SYSTEM_TABLE* SystemTable) {

    // Clear screen to the default background color.
    SystemTable->ConOut->SetAttribute (
        SystemTable->ConOut, 
        UEFI_TEXT_ATTR(DEFAULT_FOREGROUND_COLOR, DEFAULT_BACKGROUND_COLOR)
    );
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

    Memory_Map_Info memory_map_i;

    uefi_get_memory_map (SystemTable, memory_map_i);

    // Calculate the number of memory map entries.
    uint64_t num_of_mem_map_entries = memory_map_i.size / memory_map_i.desc_size;

    // Loop thru the memory map.
    for (uint64_t idx = 0; idx < num_of_mem_map_entries; idx++) {

        /* Pointer arithmetic to point to an entry in the memory map using the
        given size of the memory descriptors. */
        UEFI_MEMORY_DESCRIPTOR* mem_desc = (UEFI_MEMORY_DESCRIPTOR*)(((uint8_t*)(memory_map_i.map)) + (idx * memory_map_i.desc_size));

        uefi_printf(SystemTable, u"%u: Type:%u, Phy:%h, Virt:%h, Pages:%u, Attr:%h\r\n",
            idx,
            mem_desc->Type,
            mem_desc->PhysicalStart,
            mem_desc->VirtualStart,
            mem_desc->NumberOfPages,
            mem_desc->Attribute);

        // Wait for keystroke every 20 printed entries.
        if (((idx % 20) == 0) || (idx == (num_of_mem_map_entries - 1))) {
            uefi_wait_for_keystroke (SystemTable);
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
    
    UEFI_PRINT_ERROR (SystemTable, status, u"Could not locate GOP protocol");

    // Read the kernel binary executable into a buffer.
    uint64_t file_buffer_size;
    void* kernel_buffer = read_esp_file_into_buffer (ImageHandle, SystemTable, u"\\EFI\\BOOT\\kernel.bin", &file_buffer_size);

    /* Populate kernel handover parameters object with memory map and GOP mode
    for access to framebuffer.*/
    Kernel_Handover k;
    k.gop = *gop->Mode;
    uefi_get_memory_map (SystemTable, k.memory_map);

    PC_Screen_Font_v1_Renderer font_renderer(ImageHandle, SystemTable, u"\\EFI\\BOOT\\zap-ext-light16.psf", k.gop, 16);
    k.font_renderer = &font_renderer;
    
    /* Establish a function pointer pointing to the kinary binary's executable
    code. */
    void UEFI_API (*entry_point)(Kernel_Handover*) = NULL;
    *(void **)&entry_point = kernel_buffer; 

    uefi_printf(SystemTable, u"Address of Entry Point: %h\r\n", entry_point);

    // Exit UEFI boot services.
    SystemTable->BootServices->ExitBootServices(ImageHandle, k.memory_map.key);

    /* Call the kernel's entry point to turn control over to the operating 
    system. */
    entry_point(&k);

    // Should never reach this point in execution.
    __builtin_unreachable();

    return UEFI_SUCCESS;
}

/*******************************************************************************
MAIN FUNCTION
*******************************************************************************/
extern "C" { // Avoids name mangling of the UEFI entry point.
UEFI_STATUS UEFI_API uefi_main (UEFI_HANDLE ImageHandle, UEFI_SYSTEM_TABLE* SystemTable) {

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
        u"Read ESP",
        u"Read Memory Map",
        u"Launch cosmOS!"
    };

    // uint64_t cr0_val = read_cr0();
    // uefi_printf (SystemTable, u"CR0 value is; %h.\r\n", cr0_val);
    // uint64_t cr4_val = read_cr4();
    // uefi_printf (SystemTable, u"CR4 value is; %h.\r\n", cr4_val);

    // uefi_printf (SystemTable, u"Physical memory header size is; %u.\r\n", sizeof(physical_memory_free_header));
    // uefi_printf (SystemTable, u"Void pointer size is; %u.\r\n", sizeof(void*));

    // uefi_printf (SystemTable, u"Hello World!\r\n");

    // Memory_Map_Info* mmap_info;
    // uefi_get_memory_map (SystemTable, mmap_info);
    // Physical_Memory_Manager pmm (mmap_info, SystemTable);

    // uefi_printf (SystemTable, u"Hello World!\r\n");

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
                read_esp (ImageHandle, SystemTable);
            } else if (selected_menu_option == 1) {
                display_memory_map (ImageHandle, SystemTable);
            } else if (selected_menu_option == 2) {
                launch_cosmOS (ImageHandle, SystemTable, datetime_event);
            }
        }
    }

    return UEFI_SUCCESS;
    
}}
