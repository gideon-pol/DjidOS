#ifndef INTERFACE_TEXT_H
#define INTERFACE_TEXT_H

#include <common/common.h>
#include <kernel/memory.h>

namespace UI {
    namespace Old{
        #define INTERFACE_COLUMNS 80
        #define INTERFACE_ROWS 25

        enum VgaColor {
            Black = 0,
            Blue = 1,
            Green = 2,
            Cyan = 3,
            Red = 4,
            Magenta = 5,
            Brown = 6,
            LightGrey = 7,
            DarkGrey = 8,
            LightBlue = 9,
            LightGreen = 10,
            LightCyan = 11,
            LightRed = 12,
            Pink = 13,
            Yellow = 14,
            White = 15
        };
        
        class IChar{
            public:
                IChar(char c, VgaColor t = VgaColor::White, VgaColor b = VgaColor::Black){
                    character = c;
                    color = (t | b << 4);
                }

                VgaColor GetTextColor(){ return static_cast<VgaColor>(color & 0xF); }
                VgaColor GetBackgroundColor(){ return static_cast<VgaColor>(color & 0xF0 >> 4); }
                char GetChar(){ return character; }


            private:
                char character;
                uint8_t color;
        };

        class Cursor {
            public:
                uint8_t X = 0;
                uint8_t Y = 0;
                VgaColor CursorColor = VgaColor::White;
                VgaColor BackgroundColor = VgaColor::Black;
                IChar OriginalChar = IChar(' ', VgaColor::White, VgaColor::Black);

                Cursor(){};
                Cursor(uint8_t x, uint8_t y, VgaColor cursorColor, VgaColor backgroundColor){
                    X = x;
                    Y = y;

                    CursorColor = cursorColor;
                    BackgroundColor = backgroundColor;
                }

        };

        extern IChar* VID_MEM;

        void Clear();
        void ClearRow(uint8_t row);
        void FillRow(int row, char c = ' ', VgaColor t = VgaColor::White, VgaColor b = VgaColor::Blue);
        void Print(char* msg, int row = 0, int length = 0, VgaColor t = VgaColor::White, VgaColor b = VgaColor::Black);
        void Print(uint64_t num, int row = 0, VgaColor t = VgaColor::White, VgaColor b = VgaColor::Black);
        void PrintHex(uint64_t num, int row = 0, VgaColor t = VgaColor::White, VgaColor b = VgaColor::Black);
        void PrintByte(uint8_t num, int row = 0, VgaColor t = VgaColor::White, VgaColor b = VgaColor::Black);
        void MoveCursor(int16_t x, int16_t y);
    }
}

#endif