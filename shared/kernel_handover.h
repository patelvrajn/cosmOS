#pragma once

#include "uefi/uefi_memory_map.h"
#include "uefi/uefi.h"
#include "graphics/fonts/pc_screen_font_v1_renderer.h"

typedef struct {
    Memory_Map_Info                    memory_map; 
    UEFI_GRAPHICS_OUTPUT_PROTOCOL_MODE gop;
    PC_Screen_Font_v1_Renderer*        font_renderer;
    void*                              os_reserved_page_sets[1];
} Kernel_Handover;

