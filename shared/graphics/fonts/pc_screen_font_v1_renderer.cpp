#include "pc_screen_font_v1_renderer.h"
#include "../../uefi/uefi_file_io.h"

PC_Screen_Font_v1_Renderer::PC_Screen_Font_v1_Renderer ( 
    UEFI_HANDLE                        ImageHandle,
    UEFI_SYSTEM_TABLE*                 SystemTable, 
    char16_t*                          path, 
    UEFI_GRAPHICS_OUTPUT_PROTOCOL_MODE gop, 
    uint64_t                           font_glyph_height) {

    m_gop               = gop;
    m_font_glyph_height = font_glyph_height;

    UEFI_FILE_PROTOCOL* root = get_esp_root (ImageHandle, SystemTable);

    // Get a pointer to the file handle of the file specified in the path parameter.
    UEFI_FILE_PROTOCOL* fp;
    root->Open(root, &fp, path, UEFI_FILE_MODE_READ, 0);
    root->Close(root);

    pc_screen_font_v1_header* header;
    uint64_t header_size = sizeof(pc_screen_font_v1_header);
    SystemTable->BootServices->AllocatePool(UefiLoaderData, header_size, (void**)&header);
    fp->Read(fp, &header_size, header);

    if ((header->magic_numbers[0] != PC_SCREEN_FONT_V1_MAGIC0) || 
        (header->magic_numbers[1] != PC_SCREEN_FONT_V1_MAGIC1)) {
        return;
    }

    uint64_t glyph_buffer_size = header->character_size * 256;
    if (header->mode == 1) {
        glyph_buffer_size = header->character_size * 512;
    }

    void* glyph_buffer;
    fp->SetPosition(fp, header_size);
    SystemTable->BootServices->AllocatePool(UefiLoaderData, glyph_buffer_size, (void**)&glyph_buffer);
    fp->Read(fp, &glyph_buffer_size, glyph_buffer);

    m_font->header       = header;
    m_font->glyph_buffer = glyph_buffer;

}

void PC_Screen_Font_v1_Renderer::print_character (uint32_t color, char c, uint64_t x, uint64_t y) {

    uint32_t* fb        = (uint32_t*)m_gop.FrameBufferBase;
    uint8_t*  glyph_ptr = (((uint8_t*)m_font->glyph_buffer) + (c * m_font->header->character_size)); 

    for (uint64_t yPixel = y; yPixel < (y + m_font_glyph_height); yPixel++) {
        for (uint64_t xPixel = x; xPixel < (x + M_FONT_GLYPH_WIDTH); xPixel++) {

            if ((*glyph_ptr & (0b10000000 >> (xPixel - x))) > 0) {
                *(fb + xPixel + (yPixel * m_gop.Info->PixelsPerScanLine)) = color;
            }
        }

        glyph_ptr++;

    }
}

void PC_Screen_Font_v1_Renderer::print_string (uint32_t color, char* s, uint64_t x, uint64_t y) {

    uint64_t xPixel = x;
    char* str = s;
    
    while (*str != '\0') {
        print_character(color, *str, xPixel, y);
        xPixel += M_FONT_GLYPH_WIDTH;
        str++;
    }
}
