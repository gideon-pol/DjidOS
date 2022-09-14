#ifndef INTERFACE_GRAPHICS_H
#define INTERFACE_GRAPHICS_H

#include <common/common.h>
#include <interface/types.h>
#include <interface/graphics/terminal.h>

namespace UI{
    namespace Graphics{
        class Frame{
            public:
                Frame() = default;
                Frame(uint16_t width, uint16_t height, uintptr_t fb);
                uint16_t Width, Height;
                uint16_t GetColumns();
                uint16_t GetRows();

            private:
                uintptr_t framebuffer;
        };

        extern Frame CurrentFrame;

        typedef struct {
            uint32_t magic;
            uint32_t version;
            uint32_t header_size;
            uint32_t flags;
            uint32_t glyph_count;
            uint32_t char_size;
            uint32_t height;
            uint32_t width;
            uint32_t bytes_per_line;
        } PSF_Font;

        extern PSF_Font* Font;

        const uintptr_t FRAMEBUFFER = 0xFFFFFFD000000000;
        const uintptr_t BACKBUFFER = 0xFFFFFFD001000000;

        #define PIXEL_SIZE 4

        void Setup(uint16_t width, uint16_t height, uintptr_t framebuffer);
        void Swap();
        void DrawBox(vec2 pos, vec2 size, Color innerColor = Color::White, int outlineSize = 0, Color outlineColor = Color::White);    
        void DrawPixel(int x, int y, Color color = Color::White);
        void DrawString(char* s, int len, vec2 pos, Color textColor = Color::White, Color bgColor = Color::Transparent);
        void DrawString(char* s, int len, int column, int row, Color textColor = Color::White, Color bgColor = Color::Transparent);
        void DrawBitmap(Color* bitmap, int x, int y, uint32_t width, uint32_t height);
        void DrawBSOD(char* s);
        bool IsInitialized();
    }
}

#endif