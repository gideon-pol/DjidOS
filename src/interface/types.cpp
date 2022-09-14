#include <interface/types.h>

namespace UI{
    bool Color::operator==(const Color& other){ return R == other.R && G == other.G && B == other.B && A == other.A; }
    bool Color::operator!=(const Color& other){ return R != other.R || G != other.G || B != other.B || A != other.A; }

    uint32_t Color::ToInt(){
        return (R << 16) | (G << 8) | B;
    }
}