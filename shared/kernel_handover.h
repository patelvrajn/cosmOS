#ifndef KERNEL_HANDOVER_H
#define KERNEL_HANDOVER_H

#include "uefi/uefi_memory_map.h"
#include "uefi/uefi.h"
#include "graphics/fonts/pc_screen_font_v1_renderer.h"

typedef struct {
    Memory_Map_Info                    memory_map; 
    UEFI_GRAPHICS_OUTPUT_PROTOCOL_MODE gop;
    PC_Screen_Font_v1_Renderer*        font_renderer;
} Kernel_Handover;

#endif
/* END OF KERNEL_HANDOVER.H */
