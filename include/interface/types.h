#ifndef UI_TYPES_H
#define UI_TYPES_H

#include <common/common.h>

namespace UI{
    struct vec2{
        int X, Y;
    };

    struct vec2F{
        float X, Y;
    };

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
}

#endif