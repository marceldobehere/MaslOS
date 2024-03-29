#pragma once
#include "../base/baseComponent.h"
#include "../../../../../Window/window.h"

namespace GuiComponentStuff
{
    enum class GuiInstanceCanvasComponentAttributeType : int32_t
    {
        BG_COL = 10
    };

    class CanvasComponent : public BaseComponent
    {
        public:
        uint32_t bgColor;
        PSF1_FONT* font;

        CanvasComponent(uint32_t bgColor, ComponentSize size, BaseComponent* parent);
        void MouseClicked(MouseClickEventInfo info);
        void KeyHit(KeyHitEventInfo info);
        void Render(Field field);
        void CheckUpdates();
        void Destroy(bool destroyChildren, void (*callBackFunc)(BaseComponent* comp));
        ComponentSize GetActualComponentSize();

        void* OnMouseClickHelp;
        void (*OnMouseClickedCallBack)(void*, BaseComponent*, MouseClickEventInfo);

        void Clear();
        void Clear(bool update);
        void UpdateCanvas();
        void UpdateCanvas(int x, int y);
        void UpdateCanvas(int x, int y, int w, int h);
        void UpdateCanvas(Field field);
        

        void DrawPixel(int x, int y, uint32_t col, bool update);
        void DrawBlob(int x, int y, uint32_t col, int size);
        void DrawLine(int x1, int y1, int x2, int y2, uint32_t col, int size);
        void DrawSquare(int x, int y, int size, uint32_t col, bool fillInside);
        void DrawRect(int x, int y, int w, int h, uint32_t col, bool fillInside);
        void DrawSquare(int x, int y, int size, uint32_t col, bool fillInside, bool update);
        void DrawRect(int x, int y, int w, int h, uint32_t col, bool fillInside, bool update);
        void DrawChar(int sX, int sY, uint32_t fg,  int size, char c);
        void DrawChar(int sX, int sY, uint32_t fg, uint32_t bg, int size, char c);
        void DrawText(int sX, int sY, uint32_t fg, uint32_t bg, int size, const char* text);
        void DrawText(int sX, int sY, uint32_t col, int size, const char* text);

        bool SetAttribute(int32_t type, uint64_t val);
        uint64_t GetAttribute(int32_t type);
        int GetAttributeSize(int32_t type);
    };
}