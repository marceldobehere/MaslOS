#include "canvasComponent.h"
#include "../../../../../../osData/MStack/MStackM.h"

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
            parent->updateFields->add(Field(position, temp));

        RemoveFromStack();
    }

    void CanvasComponent::Clear()
    {
        AddToStack();
        renderer->Fill(bgColor);
        UpdateCanvas();
        RemoveFromStack();
    }

    void CanvasComponent::UpdateCanvas()
    {
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
        parent->updateFields->add(field + position);
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

}