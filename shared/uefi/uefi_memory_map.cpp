#include "uefi.h"
#include "uefi_memory_map.h"
#include "uefi_console.h"

UEFI_STATUS uefi_get_memory_map (
    UEFI_SYSTEM_TABLE* SystemTable, 
    Memory_Map_Info*   mmap
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

    mmap->size         = MemoryMapSize;
    mmap->map          = MemoryMap;
    mmap->key          = MapKey;
    mmap->desc_size    = DescriptorSize;
    mmap->desc_version = DescriptorVersion;

    return UEFI_SUCCESS;

}

uint64_t Get_Maximum_Memory_Address (Memory_Map_Info* mmap_info) {

    // Calculate the number of memory map entries.
    uint64_t num_of_mem_map_entries = mmap_info->size / mmap_info->desc_size;

    uint64_t maximum_memory_address = 0;

    // Loop thru the memory map.
    for (uint64_t idx = 0; idx < num_of_mem_map_entries; idx++) {

        /* Pointer arithmetic to point to an entry in the memory map using the
        given size of the memory descriptors. */
        UEFI_MEMORY_DESCRIPTOR* mem_desc = (UEFI_MEMORY_DESCRIPTOR*)(((uint8_t*)(mmap_info->map)) + (idx * mmap_info->desc_size));

        if (maximum_memory_address < ((mem_desc->PhysicalStart + (mem_desc->NumberOfPages * 4096)) -1)) {

            if (Is_Physical_Memory_Region_Valid (mmap_info, (mem_desc->PhysicalStart + (mem_desc->NumberOfPages * 4096)) - 1)) {

                maximum_memory_address = (mem_desc->PhysicalStart + (mem_desc->NumberOfPages * 4096)) - 1;
            }
        }
    }

    return maximum_memory_address;
}

bool Is_Physical_Memory_Region_Type_Valid (UEFI_MEMORY_TYPE mem_type) {

    return ((mem_type != UEFI_MEMORY_TYPE::UefiReservedMemoryType) && 
            (mem_type != UEFI_MEMORY_TYPE::UefiUnusableMemory)); 

}

bool Is_Physical_Memory_Region_Valid (Memory_Map_Info* mmap_info, uint64_t addr) {

    // Calculate the number of memory map entries.
    uint64_t num_of_mem_map_entries = mmap_info->size / mmap_info->desc_size;

    // Loop thru the memory map.
    for (uint64_t idx = 0; idx < num_of_mem_map_entries; idx++) {

        /* Pointer arithmetic to point to an entry in the memory map using the
        given size of the memory descriptors. */
        UEFI_MEMORY_DESCRIPTOR* mem_desc = (UEFI_MEMORY_DESCRIPTOR*)(((uint8_t*)(mmap_info->map)) + (idx * mmap_info->desc_size));

        if ((addr >= mem_desc->PhysicalStart) && (addr < (mem_desc->PhysicalStart + (mem_desc->NumberOfPages * 4096)))) {

            UEFI_MEMORY_TYPE mem_type = ((UEFI_MEMORY_TYPE)mem_desc->Type);

            return Is_Physical_Memory_Region_Type_Valid (mem_type);

        }
    }

    return false;
}
