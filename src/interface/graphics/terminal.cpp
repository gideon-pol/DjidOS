#include <interface/graphics/terminal.h>

using namespace UI::Graphics;

// Terminal currently uses a very naive method of rendering
// TODO: switch to a redrawing based approach instead of shifting up the framebuffer
namespace Terminal {
    namespace {
        uint16_t totalColumns, totalRows;
        Color textColor = Color::White;
        Color bgColor = Color::Transparent;
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
            int h = CurrentFrame.Height - Font->height * 2;
            for(int y = 0; y < h; y++){
                int row = y * CurrentFrame.Width;
                for(int x = 0; x < CurrentFrame.Width; x++){
                    *((uint32_t*)FRAMEBUFFER + x + row) = *((uint32_t*)FRAMEBUFFER + x + (y + Font->height) * CurrentFrame.Width);
                }
            }
            
            uint32_t bgColorInt = bgColor.ToInt();
            for(int y = h; y < CurrentFrame.Height - Font->height; y++){
                int row = y * CurrentFrame.Width;
                for(int x = 0; x < CurrentFrame.Width; x++){
                    *((uint32_t*)FRAMEBUFFER + x + row) = bgColorInt;
                }
            }
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
        DrawBox(0, row * Font->height, CurrentFrame.Width, CurrentFrame.Height - row * Font->height, Color::Black);
        DrawString((char*)&inputStr, inputIndex, 0, row, Color::White, Color::Black);
    }

    void Println(char* s...){
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
                        int64_t strArg = va_arg(args, int64_t);
                        if(strArg < 0){ 
                            DrawString("-", -1, column, printLine, textColor, bgColor);
                            column = printNumber(column+1, -strArg, printLine);
                        } else {
                            column = printNumber(column, strArg, printLine);
                        }
                        str++;
                        break;
                    }
                    case 'u' : {
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

    void Clear(){
        uint32_t bgColorInt = bgColor.ToInt();
        for(int x = 0; x < CurrentFrame.Width; x++){
            for(int y = 0; y < CurrentFrame.Height - Font->height; y++){
                *((uint32_t*)FRAMEBUFFER + x + y * CurrentFrame.Width) = bgColorInt;
            }
        }
    }
}