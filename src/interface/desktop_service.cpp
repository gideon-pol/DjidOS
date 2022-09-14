#include <interface/desktop_service.h>

namespace UI{
    namespace DesktopService{
        Window* windows[MAX_WINDOW_COUNT];

        int windowCount = 0;

        void* Window::operator new(size_t size, vec2 s){
            Window* window = (Window*)malloc(size);
            window->Buffer = malloc(s.X * s.Y * PIXEL_SIZE);
            window->size = s;
            return window;
        }

        vec2 Window::GetSize(){
            return size;
        }

        void Window::Update(){
            IsDirty = true;
        }

        void Window::DrawString(char* s, int len, int column, int row, Color textColor, Color bgColor){
            vec2 pos = {Position.X + column * Graphics::Font->height, Position.Y + row * Graphics::Font->height};
            Graphics::DrawString(s, len, pos, textColor, bgColor);
        }

        Spinlock windowLock = Spinlock();

        Window* CreateWindow(vec2 size){
            windowLock.Acquire();
            
            for(int i = 0; i < MAX_WINDOW_COUNT; i++){
                if(windows[i] == nullptr){
                    Window* window = new(size) Window;
                    windows[i] = window;
                    windowCount++;

                    windowLock.Unlock();
                    return windows[i];
                }
            }

            windowLock.Unlock();
            return nullptr;
        }

        void Update(){
            for(int i = 0; i < windowCount; i++){
                Window* window = windows[i];
                if(window->IsDirty){
                    uintptr_t frameBufferTarget = Graphics::FRAMEBUFFER + (window->Position.Y * Graphics::CurrentFrame.Width + window->Position.X) * PIXEL_SIZE;
                    uintptr_t windowBufferSource = (uintptr_t)window->Buffer;
                    vec2 windowSize = window->GetSize();

                    for(int y = 0; y < windowSize.Y; y++){
                        memcpy((void*)frameBufferTarget, (void*)windowBufferSource, windowSize.X * PIXEL_SIZE);
                        frameBufferTarget += Graphics::CurrentFrame.Width * PIXEL_SIZE;
                        windowBufferSource += windowSize.X * PIXEL_SIZE;
                    }

                    window->IsDirty = false;
                }
            }
        }
    }
}