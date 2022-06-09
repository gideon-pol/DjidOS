#include <interface/graphics/interface_graphics.h>
#include <interface/text/interface_text.h>

#include <kernel/memory.h>

extern char _binary_font_psf_start;
extern char _binary_font_psf_end;

namespace UI{
    namespace Graphics{
        Frame CurrentFrame;
        PSF_Font* Font;

        Frame::Frame(uint16_t width, uint16_t height, uint64_t fb) : Width(width), Height(height), framebuffer(fb) {}
        //uint16_t Frame::GetWidth(){ return frameWidth; }
        //uint16_t Frame::GetHeight(){ return frameHeight; }

        uint32_t Color::ToInt(){
            return (R << 16) | (G << 8) | B;
        }

         /*
        namespace Colors{
            Color Red = Color(255, 0, 0);
            Color Green = Color(0, 255, 0);
            Color Blue = Color(0, 0, 255);
            Color Cyan = Color(0, 255, 255);
            Color Magenta = Color(255, 0, 255);
            Color Yellow = Color(255, 255, 0);
            Color Orange = Color(255, 180, 0);
            Color Black = Color(0, 0, 0);
            Color White = Color(255, 255, 255);
            Color Transparent = Color(0, 0, 0, 0);
            uint32_t test = 0xFFFFFFFF;
        }*/

        void Setup(uint16_t width, uint16_t height, uint64_t framebuffer){
            uint64_t mappedFramebuffer = ALIGN(FRAMEBUFFER, PAGE_SIZE);

            int frameBufferBytesToMap = width * height * 4;
            int frameBufferPagesMapped = 0;

            while(frameBufferBytesToMap > 0){
                PMM::MapRegion(framebuffer + frameBufferPagesMapped * PAGE_SIZE, mappedFramebuffer + frameBufferPagesMapped * PAGE_SIZE);
                PMM::LockPages((void*)(mappedFramebuffer + frameBufferPagesMapped * PAGE_SIZE));
                frameBufferBytesToMap -= PAGE_SIZE;
                frameBufferPagesMapped++;
            }

            CurrentFrame = Frame(width, height, mappedFramebuffer);
            Font = (PSF_Font*)&_binary_font_psf_start;
            if(Font->magic != 0x864AB572) crash();
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
            Font = (PSF_Font*)&_binary_font_psf_start;
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
    }
};