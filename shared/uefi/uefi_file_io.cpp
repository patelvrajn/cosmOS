#include "uefi_console.h"

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

    UEFI_PRINT_ERROR(SystemTable, status, "Loaded Image Protocol not found on handle");

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

    UEFI_PRINT_ERROR(SystemTable, status, "Simple File System Protocol not found on handle");

    // Obtain a pointer to the file handle of the root directory of the ESP.
    UEFI_FILE_PROTOCOL* fp;
    status = sfs->OpenVolume(sfs, &fp);

    UEFI_PRINT_ERROR(SystemTable, status, "Simple File System Protocol could not open volume");

    // Return the pointer.
    return fp;
}
