#include <interface/graphics/terminal.h>

using namespace UI;
using namespace UI::Graphics;

// Terminal currently uses a very naive method of rendering
// TODO: switch to a redrawing based approach instead of shifting up the framebuffer
namespace Terminal {
    namespace {
        uint16_t totalColumns, totalRows;
        Color textColor = Color::White;
        Color bgColor = Color::Black;
        char inputStr[500];
        int inputIndex = 0;

        uint16_t printNumber(uint16_t index, uint64_t number, uint16_t row){
            char c = number % 10 + 48;
            number /= 10;
            if(number != 0) index = printNumber(index, number, row);
            DrawString(&c, 1, index, row, textColor, bgColor);
            return index + 1;
        }

        char* hexTranslation = "0123456789ABCDEF"; 
        uint8_t printNumberHex(uint8_t index, uint64_t number, uint8_t row){
            char c = hexTranslation[number % 16];
            number /= 16;
            if(number != 0) index = printNumberHex(index, number, row);
            DrawString(&c, 1, index, row, textColor, bgColor);
            return index + 1;
        }

        void createNewLine(){
            memcpy(
                    (void*)FRAMEBUFFER, 
                    (void*)FRAMEBUFFER + Font->height * CurrentFrame.Width * PIXEL_SIZE, 
                    (CurrentFrame.Width * CurrentFrame.Height - 2 * Font->height * CurrentFrame.Width) * PIXEL_SIZE);
            memset(
                    (void*)FRAMEBUFFER + (CurrentFrame.Height - Font->height * 2) * CurrentFrame.Width * PIXEL_SIZE,
                    bgColor.ToInt(), 
                    CurrentFrame.Width * Font->height * PIXEL_SIZE);
        }
    }

    void Setup(){
        totalColumns = CurrentFrame.Width / Font->width - 1;
        totalRows = CurrentFrame.Height / Font->height - 1;
    }

    void AddInputCharacter(char c){
        inputStr[inputIndex] = c;
        inputIndex++;
        int rows = CurrentFrame.GetRows();
        DrawString((char*)&inputStr, inputIndex, 0, rows-1, Color::White, Color::Black);
    }

    void RemoveInputCharacter(){
        inputIndex = max(0, --inputIndex);
        int row = CurrentFrame.GetRows() - 1;
        DrawBox({0, (int)(row * Font->height)}, {CurrentFrame.Width, (int)(CurrentFrame.Height - row * Font->height)}, Color::Black);
        DrawString((char*)&inputStr, inputIndex, 0, row, Color::White, Color::Black);
    }

    void Println(char* s, ...){
        createNewLine();
        va_list args;
        va_start(args, s);
        Print(s, 0, (totalRows-1), args);
    }

    void Print(char* s, int column, int row, ...){
        va_list args;
        va_start(args, row);
        Print(s, column, row, args);
    }

    void Print(char* s, int column, int row, va_list args){
        int strLen = strlen(s);
        char* str = s;

        while(str < s + strLen){
            if(*str == '%'){
                switch(*(str+1)){
                    case 's' : {
                        char* strArg = va_arg(args, char*);
                        DrawString(strArg, -1, column, row, textColor, bgColor);
                        str++;
                        column += strlen(strArg);
                        break;
                    }
                    case 'd' : {
                        int32_t strArg = va_arg(args, int32_t);
                        if(strArg < 0){ 
                            DrawString("-", -1, column, row, textColor, bgColor);
                            column = printNumber(column+1, (uint32_t)-strArg, row);
                        } else {
                            column = printNumber(column, (uint32_t)strArg, row);
                        }
                        str++;
                        break;
                    }
                    case 'u' : {
                        uint32_t strArg = va_arg(args, uint32_t);
                        column = printNumber(column, strArg, row);
                        str++;
                        break;
                    }
                    case 'x' : {
                        uint32_t strArg = va_arg(args, uint32_t);
                        DrawString("0x", -1, column, row, textColor, bgColor);
                        column += 2;
                        column = printNumberHex(column, strArg, row);
                        str++;
                        break;
                    }
                    case 'f' : {
                        double strArg = va_arg(args, double);
                        
                        int intPart = (int)strArg;
                        float fPart = strArg - intPart;
                        column = printNumber(column, intPart, row);

                        DrawString(".", 1, column, row, textColor, bgColor);
                        column++;

                        int decimalPrintPrecision = 2;

                        if(*(str+2) == ':'){
                            decimalPrintPrecision = *(str+3) - 48;
                            str += 2;
                        }

                        for(int i = 0; i < decimalPrintPrecision; i++){
                            fPart *= 10;
                        }

                        column = printNumber(column, (int)fPart, row);
                        str++;
                        break;
                    }

                    case 'l' : {
                        switch (*(str+2))
                        {
                            case 'd' : {
                                int64_t strArg = va_arg(args, int64_t);
                                if(strArg < 0){ 
                                    DrawString("-", -1, column, row, textColor, bgColor);
                                    column = printNumber(column+1, -strArg, row);
                                } else {
                                    column = printNumber(column, strArg, row);
                                }
                                str++;
                                break;
                            }
                            case 'u' : {
                                uint64_t strArg = va_arg(args, uint64_t);
                                column = printNumber(column, strArg, row);
                                str++;
                                break;
                            }
                            case 'x' : {
                                uint64_t strArg = va_arg(args, uint64_t);
                                DrawString("0x", -1, column, row, textColor, bgColor);
                                column += 2;
                                column = printNumberHex(column, strArg, row);
                                str++;
                                break;
                            }
                        }

                        str++;
                        break;
                    }

                    case 'c' : {
                        switch (*(str+2))
                        {
                            case 'r': textColor = Color::Red; break;
                            case 'g': textColor = Color::Green; break;
                            case 'b': textColor = Color::Blue; break;
                            case 'w': textColor = Color::White; break;
                            case '0': textColor = Color::Black; break;
                            default: textColor = Color::White; break;
                        }

                        str += 2;
                        break;
                    }

                    case '%' : {
                        DrawString("%", -1, column, row, textColor, bgColor);
                        column++;
                        str++;
                    }
                }

                str++;
                continue;   
            } else if(*str == '\n'){
                createNewLine();
                str++;
                column = 0;
                continue;
            }

            DrawString(str, 1, column, row, textColor, bgColor);
            str++;
            column++;
        }     
    }
}