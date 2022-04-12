#ifndef INTERFACE_H
#define INTERFACE_H

#include <common.h>

const size_t INTERFACE_COLUMNS = 80;
const size_t INTERFACE_ROWS = 25;

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

private:
    char character;
    uint8_t color;
};

extern IChar* VID_MEM;

class Interface{
public:
    static void Clear(){
        for(uint8_t i = 0; i < INTERFACE_ROWS; i++){
            Interface::ClearRow(i);
        }
    }

    static void ClearRow(uint8_t row){
        IChar whiteSpace = IChar(' ');
        for(uint8_t i = 0; i < INTERFACE_COLUMNS; i++){
            *(VID_MEM + i + row * INTERFACE_COLUMNS) = whiteSpace;
        }
    }

    static void FillRow(int row, char c = ' ', Color t = Color::White, Color b = Color::Blue){
        IChar ch = IChar(c, t, b);

        for(size_t i = 0; i < INTERFACE_COLUMNS; i++){
            *(VID_MEM + i + row * INTERFACE_COLUMNS) = ch;
        }
    }

    static void Print(char* msg, int row = 0, int length = 0, Color t = Color::White, Color b = Color::Black){
        int msgLength = 0;
        if(length <= 0 || strlen(msg) < length) msgLength = strlen(msg);
        else msgLength = length;
        for(size_t i = 0; i < msgLength; i++){
            IChar d = IChar(msg[i], t, b);
            *(VID_MEM + i + row * INTERFACE_COLUMNS) = d;
        }
    }

    static void Print(uint8_t num, int row = 0, Color t = Color::White, Color b = Color::Black){
        for(int8_t i = 7; i >= 0; i--){
            uint8_t remainder = num % 10;
            num /= 10;
            IChar d = IChar(remainder + 48, t, b);
            *(VID_MEM + i + row * INTERFACE_COLUMNS) = d;
            if(num == 0) break;
        }
    }

    static void PrintByte(uint8_t num, int row = 0, Color t = Color::White, Color b = Color::Black){
        for(int8_t i = 7; i >= 0; i--){
            uint8_t remainder = num % 2;
            num /= 2;
            IChar d = IChar(remainder + 48, t, b);
            *(VID_MEM + i + row * INTERFACE_COLUMNS) = d;
        }
    }

private:


};

#endif