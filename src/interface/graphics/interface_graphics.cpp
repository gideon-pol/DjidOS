#include <interface/graphics/interface_graphics.h>
#include <interface/text/interface_text.h>

#include <kernel/memory.h>

extern char _binary_src_main_psf_start;
extern char _binary_src_main_psf_end;

namespace UI{
    namespace Graphics{
        namespace {
            bool initialized = false;
        }

        Frame CurrentFrame;
        PSF_Font* Font;

        Frame::Frame(uint16_t width, uint16_t height, uintptr_t fb) : Width(width), Height(height), framebuffer(fb) {}
        uint16_t Frame::GetColumns(){ return Width/Font->width; }
        uint16_t Frame::GetRows(){ return Height/Font->height; }

        uint32_t Color::ToInt(){
            return (R << 16) | (G << 8) | B;
        }

        void Setup(uint16_t width, uint16_t height, uintptr_t framebuffer){
            uintptr_t mappedFramebuffer = ALIGN(FRAMEBUFFER, PAGE_SIZE);

            int frameBufferBytesToMap = width * height * 4;
            int frameBufferPagesMapped = 0;

            while(frameBufferBytesToMap > 0){
                VMM::MapPage((void*)(framebuffer + frameBufferPagesMapped * PAGE_SIZE), (void*)(mappedFramebuffer + frameBufferPagesMapped * PAGE_SIZE));
                PMM::LockPages(framebuffer + frameBufferPagesMapped * PAGE_SIZE);
                frameBufferBytesToMap -= PAGE_SIZE;
                frameBufferPagesMapped++;
            }

            CurrentFrame = Frame(width, height, mappedFramebuffer);
            Font = (PSF_Font*)&_binary_src_main_psf_start;
            if(Font->magic != 0x864AB572) crash();
            initialized = true;
        }

        void DrawBox(int x, int y, uint16_t sizeX, uint16_t sizeY, Color innerColor, int outlineSize, Color outlineColor){
            int startX = min(max(x, 0), (int)CurrentFrame.Width);
            int startY = min(max(y, 0), (int)CurrentFrame.Height);
 
            uint16_t endX = min(x + sizeX, (int)CurrentFrame.Width);
            uint16_t endY = min(y + sizeY, (int)CurrentFrame.Height);

            uint32_t innerColorInt = innerColor.ToInt();

            for(int i = startY; i < endY; i++){
                int row = i * CurrentFrame.Width;
                for(int j = startX; j < endX; j++){
                    *((uint32_t*)FRAMEBUFFER + row + j) = innerColorInt;
                }
            }
        }

        void DrawString(char* s, uint16_t column, uint16_t row, Color textColor, Color bgColor){
            DrawString(s, -1, row, column, textColor, bgColor);
        }

        void DrawString(char* s, int len, uint16_t column, uint16_t row, Color textColor, Color bgColor){
            Font = (PSF_Font*)&_binary_src_main_psf_start;
            uint8_t* fontTable = (uint8_t*)Font + Font->headersize;

            uint16_t bytesPerLine = (Font->width + 7) / 8;
            uint16_t xOffset = column * Font->width;
            uint16_t yOffset = row * Font->height;

            int strLen = (len == -1) ? strlen(s) : len;

            uint32_t textColorInt = textColor.ToInt();
            uint32_t bgColorInt = bgColor.ToInt();

            for(int i = 0; i < strLen; i++){
                uint8_t* glyph = (uint8_t*)(fontTable + s[i] * Font->charSize);

                for(int y = 0; y < Font->height; y++){
                    int row = (y + yOffset) * CurrentFrame.Width;

                    for(int x = 0; x < Font->width; x++){
                        if(glyph[x/8] & (0b10000000 >> (x & 0b111))){
                            *((uint32_t*)FRAMEBUFFER + row + x + xOffset) = textColorInt;
                        } else if(bgColor != Color::Transparent){
                            *((uint32_t*)FRAMEBUFFER + row + x + xOffset) = bgColorInt;
                        }
                    }

                    glyph += bytesPerLine;
                }

                xOffset += Font->width + 1;
            }
        }

        void DrawBSOD(char* s){
            DrawBox(0, 0, CurrentFrame.Width, CurrentFrame.Height, Color(0, 120, 215));
            int yOffset = (CurrentFrame.GetRows() - 6) / 2;

            DrawString(":(", -1, 20, yOffset);
            DrawString("Your PC ran into a problem and needs to restart. We're", -1, 20, yOffset+1);
            DrawString("just collecting some erro info, and then we'll restart for", -1, 20, yOffset+2);
            DrawString("you", -1, 20, yOffset+3);

            DrawString(s, -1, 20, yOffset+5);
        }

        bool IsInitialized(){
            return initialized;
        }
    }
};