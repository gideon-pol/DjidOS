#ifndef INTERFACE_GRAPHICS_H
#define INTERFACE_GRAPHICS_H

#include <common/common.h>

namespace UI{
    namespace Graphics{
        class Color{
            public:
            enum ColorEnum : uint16_t {
                Black, Blue, Green, Cyan, Red, Magenta, Brown, LightGrey, DarkGrey, Pink, Yellow, White
            };

            Color() = default;
            Color(uint8_t r, uint8_t g, uint8_t b) : R(r), G(g), B(b) {}

            constexpr Color(ColorEnum code){
                switch (code)
                {
                    case ColorEnum::Black: {R = 0; G = 0; B = 0; break;}
                    case ColorEnum::Blue: {R = 0; G = 0; B = 255; break;}
                    case ColorEnum::Green: {R = 0; G = 255; B = 0; break;}
                    case ColorEnum::Cyan: {R = 0; G = 255; B = 255; break;}
                    case ColorEnum::Red: {R = 255; G = 0; B = 0; break;}
                    default: break;
                }
            }

            uint8_t R = 0;
            uint8_t G = 0;
            uint8_t B = 0;
        };
        class Frame{
            public:
                Frame() = default;
                Frame(uint16_t width, uint16_t height, uint64_t fb);
                uint16_t GetWidth();
                uint16_t GetHeight();

            private:
                uint16_t frameWidth, frameHeight;
                uint64_t framebuffer;
        };

        typedef struct {
            uint32_t magic;
            uint32_t version;
            uint32_t headersize;
            uint32_t flags;
            uint32_t glyphCount;
            uint32_t charSize;
            uint32_t height;
            uint32_t width;
        } PSF_Font;

        extern Frame CurrentFrame;
        extern PSF_Font* font;
        const uint64_t FRAMEBUFFER = 0xFFFFFF8001000000;

        void Setup(uint16_t width, uint16_t height, uint64_t framebuffer);
        void DrawBox(int x, int y, uint16_t sizeX, uint16_t sizeY, Color innerColor = Color::White, int outlineSize = 0, Color outlineColor = Color::White);    
        void DrawString(char* s);
    }


}

#endif