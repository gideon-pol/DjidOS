#include <interface/text/interface_text.h>

namespace UI{
    namespace Old{
        IChar* VID_MEM = (IChar*)(KERNEL_OFFSET + 0xb8000);

        namespace{
            uint16_t realCursorXPos, realCursorYPos;
            Cursor cursor = Cursor(0, 0, VgaColor::Yellow, VgaColor::Red);

            void applyCursor(Cursor newCursor = cursor){
                *(VID_MEM + cursor.X + cursor.Y * INTERFACE_COLUMNS) = cursor.OriginalChar;
                IChar* newCursorChar = VID_MEM + newCursor.X + newCursor.Y * INTERFACE_COLUMNS;
                newCursor.OriginalChar = *newCursorChar;
                *newCursorChar = IChar(newCursorChar->GetChar(), VgaColor::Black, VgaColor::White);
                cursor = newCursor;
            }

            uint8_t print(uint8_t index, uint64_t number, uint8_t row){
                IChar d = IChar(number % 10 + 48);
                number /= 10;
                if(number != 0) index = print(index+1, number, row);
                else index = 0;
                *(VID_MEM + index + row * INTERFACE_COLUMNS) = d;
                return index + 1;
            }

            char* hexTranslation = "0123456789ABCDEF";

            uint8_t printHex(uint8_t index, uint64_t number, uint8_t row){
                IChar d = IChar(hexTranslation[number % 16]);
                number /= 16;
                if(number != 0) index = printHex(index+1, number, row);
                else index = 0;
                *(VID_MEM + index + row * INTERFACE_COLUMNS) = d;
                return index + 1;
            }
        }

        void Clear(){
            for(uint8_t i = 0; i < INTERFACE_ROWS; i++){
                ClearRow(i);
            }

            //applyCursor();
        }

        void ClearRow(uint8_t row){
            IChar whiteSpace = IChar(' ');
            for(uint8_t i = 0; i < INTERFACE_COLUMNS; i++){
                *(VID_MEM + i + row * INTERFACE_COLUMNS) = whiteSpace;
            }

            //applyCursor();
        }

        void FillRow(int row, char c, VgaColor t, VgaColor b){
            IChar ch = IChar(c, t, b);

            for(size_t i = 0; i < INTERFACE_COLUMNS; i++){
                *(VID_MEM + i + row * INTERFACE_COLUMNS) = ch;
            }

            //applyCursor();
        }

        void Print(char* msg, int row, int length, VgaColor t, VgaColor b){
            int msgLength = 0;
            if(length <= 0 || strlen(msg) < length) msgLength = strlen(msg);
            else msgLength = length;
            for(size_t i = 0; i < msgLength; i++){
                IChar d = IChar(msg[i], t, b);
                *(VID_MEM + i + row * INTERFACE_COLUMNS) = d;
            }

            //applyCursor();
        }

        void Print(uint64_t num, int row, VgaColor t, VgaColor b){
            print(0, num, row);
            //applyCursor();
        }

        void PrintHex(uint64_t num, int row, VgaColor t, VgaColor b){
            printHex(0, num, row);
            //applyCursor();
        }

        void PrintByte(uint8_t num, int row, VgaColor t, VgaColor b){
            for(int8_t i = 7; i >= 0; i--){
                uint8_t remainder = num % 2;
                num /= 2;
                IChar d = IChar(remainder + 48, t, b);
                *(VID_MEM + i + row * INTERFACE_COLUMNS) = d;
            }

            //applyCursor();
        }

        void MoveCursor(int16_t x, int16_t y){
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

            //applyCursor(Cursor(realCursorXPos/256, realCursorYPos/256, cursor.CursorColor, cursor.BackgroundColor));
        }
    }
}