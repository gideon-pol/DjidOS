#ifndef INTERFACE_GRAPHICS_H
#define INTERFACE_GRAPHICS_H

#include <common/common.h>
#include <interface/graphics/terminal.h>

namespace UI{
    namespace Graphics{
        class Color{
            public:
            
            enum ColorEnum : uint16_t {
                Black, Blue, Green, Cyan, Red, Orange, Magenta, Brown, LightGrey, DarkGrey, Pink, Yellow, White, Transparent
            };

            Color() {}
            Color(uint8_t r, uint8_t g, uint8_t b) : R(r), G(g), B(b) { };
            Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : R(r), G(g), B(b), A(a) {}

            bool operator ==(const Color& other);
            bool operator !=(const Color& other);
/*
            static Color Black;
            static Color Blue;
            static Color Green;
            static Color Cyan;
            static Color Red;
            static Color Orange;
            static Color White;
            static Color Transparent;*/
            uint32_t ToInt();
            
            constexpr Color(ColorEnum code){
                switch (code)
                {
                    case ColorEnum::Black: R = 0; G = 0; B = 0; break;
                    case ColorEnum::Blue: R = 0; G = 0; B = 255; break;
                    case ColorEnum::Green: R = 0; G = 255; B = 0; break;
                    case ColorEnum::Cyan: R = 0; G = 255; B = 255; break;
                    case ColorEnum::Red: R = 255; G = 0; B = 0; break;
                    case ColorEnum::Orange: R = 255; G = 180; B = 0; break;
                    case ColorEnum::White: R = 255; G = 255; B = 255; break;
                    case ColorEnum::Transparent: R = 0; G = 0; B = 0; A = 0; break;

                    default: break;
                }  
            }

            static Color FromHSV(int hue, float saturation, float value){
                Color out;

                if(saturation <= 0.0) {
                    return Color(value, value, value);
                }

                double hh = hue % 360;
                hh /= 60.0;
                long i = (long)hh;
                double ff = hh - i;
                double p = value * (1.0 - saturation);
                double q = value * (1.0 - (saturation * ff));
                double t = value * (1.0 - (saturation * (1.0 - ff)));

                p *= 255;
                t *= 255;
                q *= 255;
                value *= 255;

                switch(i) {
                case 0:
                    out.R = value;
                    out.G = t;
                    out.B = p;
                    break;
                case 1:
                    out.R = q;
                    out.G = value;
                    out.B = p;
                    break;
                case 2:
                    out.R = p;
                    out.G = value;
                    out.B = t;
                    break;

                case 3:
                    out.R = p;
                    out.G = q;
                    out.B = value;
                    break;
                case 4:
                    out.R = t;
                    out.G = p;
                    out.B = value;
                    break;
                case 5:
                default:
                    out.R = value;
                    out.G = p;
                    out.B = q;
                    break;
                }

                return out;     
            }

            uint8_t R = 0;
            uint8_t G = 0;
            uint8_t B = 0;
            uint8_t A = 255;
        };

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

        extern Frame CurrentFrame;
        extern PSF_Font* Font;

        const uintptr_t FRAMEBUFFER = 0xFFFFFFD000000000;
        const uintptr_t BACKBUFFER = 0xFFFFFFD000400000;

        #define PIXEL_SIZE 4

        void Setup(uint16_t width, uint16_t height, uintptr_t framebuffer);
        void DrawBox(int x, int y, uint16_t sizeX, uint16_t sizeY, Color innerColor = Color::White, int outlineSize = 0, Color outlineColor = Color::White);    
        void DrawPixel(int x, int y, Color color = Color::White);
        void DrawString(char* s, int len, int column, int row, Color textColor = Color::White, Color bgColor = Color::Transparent);
        void DrawBitmap(Color* bitmap, int x, int y, uint32_t width, uint32_t height);
        void DrawBSOD(char* s);
        bool IsInitialized();
    }
}

#endif