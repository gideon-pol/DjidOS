#include <interface/graphics/interface_graphics.h>
#include <interface/text/interface_text.h>

#include <kernel/memory.h>

extern char _binary_font_psf_start;
extern char _binary_font_psf_end;

namespace UI{
    namespace Graphics{
        Frame CurrentFrame;
        PSF_Font* font;

        Frame::Frame(uint16_t width, uint16_t height, uint64_t fb) : frameWidth(width), frameHeight(height), framebuffer(fb) {}
        uint16_t Frame::GetWidth(){ return frameWidth; }
        uint16_t Frame::GetHeight(){ return frameHeight; }

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
            font = (PSF_Font*)&_binary_font_psf_start;
            if(font->magic != 0x864AB572) crash();
        }

        void DrawBox(int x, int y, uint16_t sizeX, uint16_t sizeY, Color innerColor, int outlineSize, Color outlineColor){
            int startX = min(max(x, 0), (int)CurrentFrame.GetWidth());
            int startY = min(max(y, 0), (int)CurrentFrame.GetHeight());

            uint16_t endX = min(x + sizeX, (int)CurrentFrame.GetWidth());
            uint16_t endY = min(y + sizeY, (int)CurrentFrame.GetHeight());

            uint32_t innerColorInt = (innerColor.R << 16) | (innerColor.G << 8) | innerColor.B;

            for(int i = startY; i < endY; i++){
                int row = i * CurrentFrame.GetWidth();
                for(int j = startX; j < endX; j++){
                    *((uint32_t*)FRAMEBUFFER + row + j) = innerColorInt;
                }
            }
        }

        void DrawString(char* s){
            font = (PSF_Font*)&_binary_font_psf_start;
            uint8_t* fontTable = (uint8_t*)font + font->headersize;

            UI::Old::PrintHex(font->magic, 0);
            UI::Old::Print(font->version, 1);
            UI::Old::Print(font->headersize, 2);
            UI::Old::Print(font->flags, 3);
            UI::Old::Print(font->glyphCount, 4);
            UI::Old::Print(font->charSize, 5);
            UI::Old::Print(font->width, 6);
            UI::Old::Print(font->height, 7);
            UI::Old::PrintHex((uint64_t)&_binary_font_psf_start, 8);

            uint16_t bytesPerLine = (font->width + 7) / 8;

            uint16_t stringLength = strlen(s);

            uint16_t xOffset = 0;

            for(int i = 0; i < stringLength; i++){
                uint8_t* glyph = (uint8_t*)(fontTable + s[i] * font->charSize);

                for(int y = 0; y < font->height; y++){
                    int row = y * CurrentFrame.GetWidth();
                    uint16_t mask = 0b1 << (font->width - 1);

                    for(int x = 0; x < font->width; x++){
                        if(*(uint32_t*)glyph & mask){
                            *((uint32_t*)FRAMEBUFFER + row + x + xOffset) = 0xFFFFFF;
                        }
                        mask >>= 1;
                    }

                    glyph += bytesPerLine;
                }

                xOffset += font->width + 1;
            }
        }
    }
};