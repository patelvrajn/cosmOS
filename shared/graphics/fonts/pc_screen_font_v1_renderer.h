#pragma once
#include <stdint.h>
#include "../../uefi/uefi.h"

#define PC_SCREEN_FONT_V1_MAGIC0 0x36
#define PC_SCREEN_FONT_V1_MAGIC1 0x04

typedef struct {
    uint8_t magic_numbers[2];
    uint8_t mode;
    uint8_t character_size;
} pc_screen_font_v1_header;

typedef struct {
    pc_screen_font_v1_header* header;
    void*                     glyph_buffer;
} pc_screen_font_v1_font;

class PC_Screen_Font_v1_Renderer {

    public:

        PC_Screen_Font_v1_Renderer ( 
            UEFI_HANDLE                        ImageHandle,
            UEFI_SYSTEM_TABLE*                 SystemTable, 
            char16_t*                          path, 
            UEFI_GRAPHICS_OUTPUT_PROTOCOL_MODE gop, 
            uint64_t                           font_glyph_height);

        void print_character (uint32_t color, char c,  uint64_t x, uint64_t y);
        void print_string    (uint32_t color, char* s, uint64_t x, uint64_t y);

    private:

        pc_screen_font_v1_font*            m_font;
        UEFI_GRAPHICS_OUTPUT_PROTOCOL_MODE m_gop;
        static const uint8_t M_FONT_GLYPH_WIDTH = 8;
        uint64_t m_font_glyph_height;

};
