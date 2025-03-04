#pragma once
#include "uefi.h"

typedef struct {
    uint64_t                size;
    UEFI_MEMORY_DESCRIPTOR* map;
    uint64_t                key;
    uint64_t                desc_size;
    uint32_t                desc_version;
} Memory_Map_Info;

UEFI_STATUS uefi_get_memory_map (
    UEFI_SYSTEM_TABLE* SystemTable, 
    Memory_Map_Info*   mmap
);

uint64_t Get_Maximum_Memory_Address (Memory_Map_Info* mmap_info);
bool Is_Physical_Memory_Region_Type_Valid (UEFI_MEMORY_TYPE mem_type);
bool Is_Physical_Memory_Region_Valid (Memory_Map_Info* mmap_info, uint64_t addr);
