#ifndef INTERFACE_H
#define INTERFACE_H

#include <common.h>

#define INTERFACE_COLUMNS 80
#define INTERFACE_ROWS 25

enum Color{
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
    IChar(char c, Color t = Color::White, Color b = Color::Black){
        character = c;
        color = (t | b << 4);
    }

    Color GetTextColor(){ return static_cast<Color>(color & 0xF); }
    Color GetBackgroundColor(){ return static_cast<Color>(color & 0xF0 >> 4); }
    char GetChar(){ return character; }


private:
    char character;
    uint8_t color;
};

class Cursor {
public:
    uint8_t X = 0;
    uint8_t Y = 0;
    Color CursorColor = Color::White;
    Color BackgroundColor = Color::Black;
    IChar OriginalChar = IChar(' ', Color::White, Color::Black);

    Cursor(){};
    Cursor(uint8_t x, uint8_t y, Color cursorColor, Color backgroundColor){
        X = x;
        Y = y;

        CursorColor = cursorColor;
        BackgroundColor = backgroundColor;
    }

private:
};

extern IChar* VID_MEM;

uint8_t print(uint8_t index, uint64_t number, uint8_t row);

class Interface{
public:
    static void Clear(){
        for(uint8_t i = 0; i < INTERFACE_ROWS; i++){
            Interface::ClearRow(i);
        }

        //applyCursor();
    }

    static void ClearRow(uint8_t row){
        IChar whiteSpace = IChar(' ');
        for(uint8_t i = 0; i < INTERFACE_COLUMNS; i++){
            *(VID_MEM + i + row * INTERFACE_COLUMNS) = whiteSpace;
        }

        applyCursor();
    }

    static void FillRow(int row, char c = ' ', Color t = Color::White, Color b = Color::Blue){
        IChar ch = IChar(c, t, b);

        for(size_t i = 0; i < INTERFACE_COLUMNS; i++){
            *(VID_MEM + i + row * INTERFACE_COLUMNS) = ch;
        }

        applyCursor();
    }

    static void Print(char* msg, int row = 0, int length = 0, Color t = Color::White, Color b = Color::Black){
        int msgLength = 0;
        if(length <= 0 || strlen(msg) < length) msgLength = strlen(msg);
        else msgLength = length;
        for(size_t i = 0; i < msgLength; i++){
            IChar d = IChar(msg[i], t, b);
            *(VID_MEM + i + row * INTERFACE_COLUMNS) = d;
        }

        //applyCursor();
    }

    static void Print(uint64_t num, int row = 0, Color t = Color::White, Color b = Color::Black){
        print(0, num, row);
        //applyCursor();
    }

    static void PrintByte(uint8_t num, int row = 0, Color t = Color::White, Color b = Color::Black){
        for(int8_t i = 7; i >= 0; i--){
            uint8_t remainder = num % 2;
            num /= 2;
            IChar d = IChar(remainder + 48, t, b);
            *(VID_MEM + i + row * INTERFACE_COLUMNS) = d;
        }

        applyCursor();
    }

    static void MoveCursor(int16_t x, int16_t y){
        if(realCursorXPos + x < 0) realCursorXPos = 0;
        else if(realCursorXPos + x >= 256 * INTERFACE_COLUMNS) realCursorXPos = 256 * (INTERFACE_COLUMNS - 1);
        else realCursorXPos = realCursorXPos + x;

        if(realCursorYPos + y < 0) realCursorYPos = 0;
        else if(realCursorYPos + y >= 256 * INTERFACE_ROWS) realCursorYPos = 256 * (INTERFACE_ROWS - 1);
        else realCursorYPos = realCursorYPos + y;

        /*
        uint8_t newX, newY;
        if((int16_t)cursor.X + x < 0) newX = 0;
        else if((int16_t)cursor.X + x >= INTERFACE_COLUMNS) newX = INTERFACE_COLUMNS - 1;
        else newX = cursor.X + x;

        if((int16_t)cursor.Y + y < 0) newY = 0;
        else if((int16_t)cursor.Y + y >= INTERFACE_ROWS) newY = INTERFACE_ROWS - 1;
        else newY = cursor.Y + y;*/

        applyCursor(Cursor(realCursorXPos/256, realCursorYPos/256, cursor.CursorColor, cursor.BackgroundColor));
    }

private:
    inline static uint16_t realCursorXPos, realCursorYPos;
    inline static Cursor cursor = Cursor(0, 0, Color::Yellow, Color::Red);
    static void applyCursor(Cursor newCursor = cursor){
        *(VID_MEM + cursor.X + cursor.Y * INTERFACE_COLUMNS) = cursor.OriginalChar;
        IChar* newCursorChar = VID_MEM + newCursor.X + newCursor.Y * INTERFACE_COLUMNS;
        newCursor.OriginalChar = *newCursorChar;
        *newCursorChar = IChar(newCursorChar->GetChar(), Color::Black, Color::White);
        cursor = newCursor;
    }
};

#endif