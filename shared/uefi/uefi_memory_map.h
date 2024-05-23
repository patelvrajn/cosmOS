#ifndef UEFI_MEMORY_MAP_H
#define UEFI_MEMORY_MAP_H
#include "uefi.h"

typedef struct {
    uint64_t               size;
    UEFI_MEMORY_DESCRIPTOR *map;
    uint64_t               key;
    uint64_t               desc_size;
    uint32_t               desc_version;
} Memory_Map_Info;

UEFI_STATUS uefi_get_memory_map (
    UEFI_SYSTEM_TABLE* SystemTable, 
    Memory_Map_Info*   mmap
);

#endif
/* END OF UEFI_MEMORY_MAP.H */
