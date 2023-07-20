#include "canvasComponent.h"
#include "../../../../../../osData/MStack/MStackM.h"
#include "../../../../../../osData/osData.h"

namespace GuiComponentStuff
{
    CanvasComponent::CanvasComponent(uint32_t bgColor, ComponentSize size, BaseComponent* parent)
    {
        RenderFunc = (void (*)(void*, Field))&Render;
        CheckUpdatesFunc = (void (*)(void*))&CheckUpdates;
        MouseClickedFunc = (void (*)(void*, MouseClickEventInfo))&MouseClicked;
        KeyHitFunc = (void (*)(void*, KeyHitEventInfo))&KeyHit;
        DestroyFunc = (void (*)(void*, bool, void (*)(BaseComponent* comp)))&Destroy;
        GetActualComponentSizeFunc = (ComponentSize (*)(void*))&GetActualComponentSize;
        SetAttributeFunc = (bool (*)(void*, int32_t, uint64_t))&SetAttribute;
        GetAttributeFunc = (uint64_t (*)(void*, int32_t))&GetAttribute;
        GetAttributeSizeFunc = (int (*)(void*, int32_t))&GetAttributeSize;

        font = osData.debugTerminalWindow->renderer->psf1_font; // just steal default font
        this->bgColor = bgColor;
        this->size = size;
        componentType = CANVAS;
        this->parent = parent;
        ComponentSize temp = GetActualComponentSize();
        renderer = new ComponentRenderer(temp);
        renderer->bgCol = bgColor;
        renderer->Fill(bgColor);
        oldPosition = position;
        oldSize = temp;
        updateFields = new List<Field>(5);

        OnMouseClickedCallBack = NULL;
        OnMouseClickHelp = NULL;

        CheckUpdates();
        //Render(Field(Position(), GetActualComponentSize()));
    }

    void CanvasComponent::MouseClicked(MouseClickEventInfo info)
    {
        if (OnMouseClickedCallBack != NULL)
            OnMouseClickedCallBack(OnMouseClickHelp, this, info);
    }

    void CanvasComponent::KeyHit(KeyHitEventInfo info)
    {

    }

    void CanvasComponent::CheckUpdates()
    {
        AddToStack();
        bool update = false;
        ComponentSize temp = GetActualComponentSize();
        if (oldSize != temp)
        {
            renderer->bgCol = bgColor;
            renderer->Resize(temp);
            //renderer->Fill(bgColor);
            oldSize = temp;
            update = true;
        }

        

        if (update)
        {
            UpdateCanvas();
        }

        while (updateFields->getCount() > 0)
        {
            Field tempField = updateFields->lastElement();
            updateFields->removeLast();
            parent->updateFields->add(tempField + position);
        }
            
        

        RemoveFromStack();
    }

    void CanvasComponent::Clear()
    {
        Clear(true);
    }

    void CanvasComponent::Clear(bool update)
    {
        AddToStack();
        renderer->Fill(bgColor);
        if (update)
            UpdateCanvas();
        RemoveFromStack();
    }

    void CanvasComponent::UpdateCanvas()
    {
        updateFields->clear();
        UpdateCanvas(Field(Position(), GetActualComponentSize()));
    }
    void CanvasComponent::UpdateCanvas(int x, int y)
    {
        UpdateCanvas(Field(Position(x, y), ComponentSize(1,1)));
    }
    void CanvasComponent::UpdateCanvas(int x, int y, int w, int h)
    {
        UpdateCanvas(Field(Position(x, y), ComponentSize(w, h)));
    }
    void CanvasComponent::UpdateCanvas(Field field)
    {
        AddToStack();
        updateFields->add(field);
        RemoveFromStack();
    }

    void CanvasComponent::Render(Field field)
    {
        AddToStack();
        if (!hidden)
            renderer->Render(position, field, parent->renderer->componentFrameBuffer);
        RemoveFromStack();
    }

    void CanvasComponent::Destroy(bool destroyChildren, void (*callBackFunc)(BaseComponent* comp))
    {
        AddToStack();
        if (callBackFunc != NULL)
            callBackFunc(this);
        renderer->Free();
        updateFields->free();
        _Free(updateFields);
        RemoveFromStack();
    }

    ComponentSize CanvasComponent::GetActualComponentSize()
    {
        if (size.IsXFixed && size.IsYFixed)
            return size;

        ComponentSize temp = ComponentSize(0, 0);
        if (size.IsXFixed)
            temp.FixedX = size.FixedX;
        else
            temp.FixedX = size.ScaledX * parent->GetActualComponentSize().FixedX;

        if (size.IsYFixed)
            temp.FixedY = size.FixedY;
         else
            temp.FixedY = size.ScaledY * parent->GetActualComponentSize().FixedY;

        if (temp.FixedX < 0)
            temp.FixedX = 0;
        if (temp.FixedY < 0)
            temp.FixedY = 0;

        return temp;
    }

    bool CanvasComponent::SetAttribute(int32_t type, uint64_t val)
    {
        if (type == 10)
        {
            bgColor = *((uint32_t*)&val);
            return true;
        }

        return false;
    }

    uint64_t CanvasComponent::GetAttribute(int32_t type)
    {
        uint8_t temp[8];
        for (int i = 0; i < 8; i++)
            temp[i] = 0;

        if (type == 10)
            *((uint32_t*)temp) = bgColor;

        return *((uint64_t*)temp);
    }

    int CanvasComponent::GetAttributeSize(int32_t type)
    {
        if (type == 10)
            return 4;

        return 0;
    }

    void CanvasComponent::DrawPixel(int x, int y, uint32_t col, bool update)
    {
        GuiComponentStuff::ComponentFramebuffer* fb = renderer->componentFrameBuffer;
        if (x >= 0 && x < fb->Width && 
            y >= 0 && y < fb->Height)
            fb->pixels[x + y * fb->Width] = col;
        if (update)
            UpdateCanvas(x, y);
    }
    void CanvasComponent::DrawBlob(int x, int y, uint32_t col, int size)
    {
        for (int x1 = x - size; x1 <= x + size; x1++)
            for (int y1 = y - size; y1 <= y + size; y1++)
                DrawPixel(x1, y1, col, false);
        
        UpdateCanvas(x - size, y - size, 1+size*2, 1+size*2);
    }
    void CanvasComponent::DrawLine(int x1, int y1, int x2, int y2, uint32_t col, int size)
    {
        if (x1 == x2 || y1 == y2)
        {
            // fill with rect directly to make fast
            if (x1 > x2)
            {
                int temp = x1;
                x1 = x2;
                x2 = temp;
            }

            if (y1 > y2)
            {
                int temp = y1;
                y1 = y2;
                y2 = temp;
            }

            DrawRect(
                x1 - size, 
                y1 - size, 
                x2 - x1 + 1 + size * 2, 
                y2 - y1 + 1 + size * 2, 
                col, true, true);
        }
        else
        {
            int dx = x2 - x1;
            int dy = y2 - y1;
            int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
            // float xInc = dx / (float)steps;
            // float yInc = dy / (float)steps;

            for (int i = 0; i < steps; i++)
            {
                DrawBlob(
                    (x1 * steps + (dx * i)) / steps, 
                    (y1 * steps + (dy * i)) / steps,
                    col, 
                    size);
            }
        }
    }

    void CanvasComponent::DrawSquare(int x, int y, int size, uint32_t col, bool fillInside)
    {
        DrawRect(x, y, size, size, col, fillInside, true);
    }

    void CanvasComponent::DrawRect(int x, int y, int w, int h, uint32_t col, bool fillInside)
    {
        DrawRect(x, y, w, h, col, fillInside, true);
    }

    void CanvasComponent::DrawSquare(int x, int y, int size, uint32_t col, bool fillInside, bool update)
    {
        DrawRect(x, y, size, size, col, fillInside, update);
    }


    void CanvasComponent::DrawRect(int x, int y, int w, int h, uint32_t col, bool fillInside, bool update)
    {
        if (fillInside)
        {
            for (int x1 = x; x1 < x + w; x1++)
                for (int y1 = y; y1 < y + h; y1++)
                    DrawPixel(x1, y1, col, false);
        }
        else
        {
            for (int x1 = x; x1 < x + w; x1++)
            {
                DrawPixel(x1, y, col, false);
                DrawPixel(x1, y + h - 1, col, false);
            }
            for (int y1 = y; y1 < y + h; y1++)
            {
                DrawPixel(x, y1, col, false);
                DrawPixel(x + w - 1, y1, col, false);
            }
        }

        if (update)
            UpdateCanvas(x, y, w, h);
    }


    void CanvasComponent::DrawChar(int sX, int sY, uint32_t fg,  int size, char c)
    {
        // this is used for all scales
        GuiComponentStuff::ComponentFramebuffer* fb = renderer->componentFrameBuffer;
        char *fontPtr = ((char *)font->glyphBuffer) + (c * font->psf1_Header->charsize);

        for (int64_t y = sY; y < sY + 16 * size; y += size)
        {
            for (int64_t x = sX; x < sX + 8 * size; x += size)
            {               
                if ((*fontPtr & (0b10000000 >> ((x - sX) / size))) > 0)
                    DrawSquare(x, y, size, fg, true, false);
            }
            fontPtr++;
        }
        UpdateCanvas(sX, sY, 8 * size, 16 * size);
    }

    void CanvasComponent::DrawChar(int sX, int sY, uint32_t fg, uint32_t bg, int size, char c)
    {
        DrawRect(sX, sY, size * 8, size * 16, bg, true);
        DrawChar(sX, sY, fg, size, c);
    }

    void CanvasComponent::DrawText(int sX, int sY, uint32_t col, int size, const char* text)
    {
        int len = StrLen(text);
        int x = sX;
        int y = sY;
        for (int i = 0; i < len; i++)
        {
            if (text[i] == '\n')
            {
                y += size * 16;
                x = sX;
                continue;
            }
            
            DrawChar(x, y, col, size, text[i]);
            x += size * 8;
        }
    }

    void CanvasComponent::DrawText(int sX, int sY, uint32_t fg, uint32_t bg, int size, const char* text)
    {
        int len = StrLen(text);
        int x = sX;
        int y = sY;
        for (int i = 0; i < len; i++)
        {
            if (text[i] == '\n')
            {
                y += size * 16;
                x = sX;
                continue;
            }
            
            DrawChar(x, y, fg, bg, size, text[i]);
            x += size * 8;
        }
    }

}