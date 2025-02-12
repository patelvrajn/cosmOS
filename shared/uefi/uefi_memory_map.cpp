#include "uefi.h"
#include "uefi_memory_map.h"
#include "uefi_console.h"

UEFI_STATUS uefi_get_memory_map (
    UEFI_SYSTEM_TABLE* SystemTable, 
    Memory_Map_Info&   mmap
) {

    uint64_t                MemoryMapSize     = 0;
    UEFI_MEMORY_DESCRIPTOR* MemoryMap         = nullptr;
    uint64_t                MapKey            = 0;
    uint64_t                DescriptorSize    = 0;
    uint32_t                DescriptorVersion = 0;

    // Get initial memory map size, expecting buffer too small error.
    UEFI_STATUS status = UEFI_SUCCESS;
    status = SystemTable->BootServices->GetMemoryMap (&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);

    // Allocate buffer for actual memory map from given size. Additional buffer
    // size needs to be allocated equal to 2 additional memory descriptors due 
    // to this allocation itself (could cause a memory segment to split into 2).
    MemoryMapSize += (DescriptorSize * 2);  
    status = SystemTable->BootServices->AllocatePool (
        UefiLoaderData, 
        MemoryMapSize,
        (void **)&MemoryMap
    );

    UEFI_PRINT_ERROR(SystemTable, status, u"Could not allocate buffer for memory map");

    // Call GetMemoryMap again to get the memory map now that the buffer is the
    // correct size.
    status = SystemTable->BootServices->GetMemoryMap (&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);

    UEFI_PRINT_ERROR(SystemTable, status, u"Could not obtain memory map!");

    mmap.size         = MemoryMapSize;
    mmap.map          = MemoryMap;
    mmap.key          = MapKey;
    mmap.desc_size    = DescriptorSize;
    mmap.desc_version = DescriptorVersion;

    return UEFI_SUCCESS;

}
