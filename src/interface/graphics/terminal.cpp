#include <interface/graphics/terminal.h>

using namespace UI::Graphics;

namespace Terminal {
    namespace {
        uint16_t totalColumns, totalRows;
        Color textColor = Color::White;
        Color bgColor = Color::Transparent;

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
            for(int x = 0; x < CurrentFrame.Width; x++){
                for(int y = 0; y < CurrentFrame.Height; y++){
                    *((uint32_t*)FRAMEBUFFER + x + y * CurrentFrame.Width) = *((uint32_t*)FRAMEBUFFER + x + (y + Font->height) * CurrentFrame.Width);
                }
            }
        }
    }

    void Setup(){
        totalColumns = CurrentFrame.Width / Font->width - 1;
        totalRows = CurrentFrame.Height / Font->height - 1;
    }

    void PrintLine(char* s...){
        va_list args;
        va_start(args, s);

        int column = 0;
        int strLen = strlen(s);
        char* str = s;

        int printLine = totalRows - 1;
        
        createNewLine();

        while(str < s + strLen){
            if(*str == '%'){
                switch(*(str+1)){
                    case 's' : {
                        char* strArg = va_arg(args, char*);
                        DrawString(strArg, -1, column, printLine, textColor, bgColor);
                        str++;
                        column += strlen(strArg);
                        break;
                    }
                    case 'd' : {
                        uint64_t strArg = va_arg(args, uint64_t);
                        column = printNumber(column, strArg, printLine);
                        str++;
                        break;
                    }
                    case 'x' : {
                        uint64_t strArg = va_arg(args, uint64_t);
                        DrawString("0x", -1, column, printLine, textColor, bgColor);
                        column += 2;
                        column = printNumberHex(column, strArg, printLine);
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
                }

                str++;
                continue;   
            } else if(*str == '\n'){
                createNewLine();
                str++;
                column = 0;
                continue;
            }

            DrawString(str, 1, column, printLine, textColor, bgColor);
            str++;
            column++;

        }
    }
}