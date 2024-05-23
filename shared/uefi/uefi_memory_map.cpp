#include "uefi.h"
#include "uefi_memory_map.h"

UEFI_STATUS uefi_get_memory_map (
    UEFI_SYSTEM_TABLE* SystemTable, 
    Memory_Map_Info*   mmap
) {

    // Get initial memory map size, expecting buffer too small error.
    UEFI_STATUS status = UEFI_SUCCESS;
    status = SystemTable->BootServices->GetMemoryMap (
        &mmap->size,
        mmap->map,
        &mmap->key,
        &mmap->desc_size,
        &mmap->desc_version);

    // Allocate buffer for actual memory map from given size. Additional buffer
    // size needs to be allocated equal to 2 additional memory descriptors due 
    // to this allocation itself (could cause a memory segment to split into 2).
    mmap->size += (mmap->desc_size * 2);  
    status = SystemTable->BootServices->AllocatePool (
        UefiLoaderData, 
        mmap->size,
        (void **)&mmap->map
    );

    // Call GetMemoryMap again to get the memory map now that the buffer is the
    // correct size.
    status = SystemTable->BootServices->GetMemoryMap (
        &mmap->size,
        mmap->map,
        &mmap->key,
        &mmap->desc_size,
        &mmap->desc_version
    );

    return UEFI_SUCCESS;
}
