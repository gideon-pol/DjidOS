#ifndef DESKTOP_SERIVCE_H
#define DESKTOP_SERIVCE_H

#include <interface/types.h>
#include <kernel/scheduling/locking.h>

namespace UI {
    namespace DesktopService{
        #define MAX_WINDOW_COUNT 64
        class Window{
            public:
                void* operator new(size_t size, vec2 s);

                vec2 GetSize();
                void Update();

                void DrawString(char* s, int len, int column, int row, Color textColor = Color::White, Color bgColor = Color::Transparent);

                void* Buffer;
                vec2 Position = {0, 0};
                bool IsDirty = true;
            private:
                vec2 size;
        };

        Window* CreateWindow(vec2 size);
        void Update();
    }
}


#endif