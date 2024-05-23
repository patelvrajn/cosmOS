#ifndef KERNEL_HANDOVER_H
#define KERNEL_HANDOVER_H

#include "uefi/uefi_memory_map.h"
#include "uefi/uefi.h"

typedef struct {
    Memory_Map_Info                    memory_map; 
    UEFI_GRAPHICS_OUTPUT_PROTOCOL_MODE gop;
} Kernel_Handover;

#endif
/* END OF KERNEL_HANDOVER.H */
