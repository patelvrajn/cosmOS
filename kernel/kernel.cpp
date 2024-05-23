#include "../shared/uefi/uefi.h"
#include "../shared/uefi/uefi_memory_map.h"
#include "../shared/kernel_handover.h"

extern "C" { // Avoids name mangling of the UEFI entry point.
__attribute__((section(".kernel"))) int UEFI_API kernel_main (Kernel_Handover* k) {

    // Pointer to framebuffer in memory.
    uint32_t* framebuffer = (uint32_t*) k->gop.FrameBufferBase; 

    // Horizontal and vertical resolution.
    uint32_t x_resolution = k->gop.Info->PixelsPerScanLine;
    uint32_t y_resolution = k->gop.Info->VerticalResolution;

    // Color entire framebuffer.
    for (uint32_t y = 0; y < y_resolution; y++)
        for (uint32_t x = 0; x < x_resolution; x++)
            framebuffer[((y * x_resolution) + x)] = 0xFFDDDDDD;

    // Never return to UEFI.
    while(1);

    return 0;

}}
