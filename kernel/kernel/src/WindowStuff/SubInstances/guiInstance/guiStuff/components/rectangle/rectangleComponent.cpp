#include "rectangleComponent.h"
#include "../../../../../../osData/MStack/MStackM.h"

namespace GuiComponentStuff
{
    RectangleComponent::RectangleComponent(uint32_t fillColor, ComponentSize size, BaseComponent* parent)
    {
        this->fillColor = fillColor;
        this->size = size;
        componentType = RECT;
        this->parent = parent;
        ComponentSize temp = GetActualComponentSize();
        renderer = new ComponentRenderer(temp);
        renderer->bgCol = fillColor;
        renderer->Fill(fillColor);
        oldPosition = position;
        oldSize = temp;
        oldFillColor = fillColor;
        updateFields = new List<Field>(5);

        CheckUpdates();
        //Render(Field(Position(), GetActualComponentSize()));
    }

    void RectangleComponent::MouseClicked(MouseClickEventInfo info)
    {

    }

    void RectangleComponent::KeyHit(KeyHitEventInfo info)
    {

    }

    void RectangleComponent::CheckUpdates()
    {
        AddToStack();
        bool update = false;
        ComponentSize temp = GetActualComponentSize();
        if (oldSize != temp)
        {
            renderer->Resize(temp);
            renderer->Fill(fillColor);
            oldSize = temp;
            update = true;
        }
        if (oldFillColor != fillColor)
        {
            renderer->Fill(fillColor);
            oldFillColor = fillColor;
            update = true;
        }

        if (update)
            parent->updateFields->add(Field(position, temp));

        RemoveFromStack();
    }

    void RectangleComponent::Render(Field field)
    {
        AddToStack();
        if (!hidden)
            renderer->Render(position, field, parent->renderer->componentFrameBuffer);
        RemoveFromStack();
    }

    void RectangleComponent::Destroy(bool destroyChildren, void (*callBackFunc)(BaseComponent* comp))
    {
        AddToStack();
        if (callBackFunc != NULL)
            callBackFunc(this);
        renderer->Free();
        updateFields->free();
        _Free(updateFields);
        RemoveFromStack();
    }

    ComponentSize RectangleComponent::GetActualComponentSize()
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

    bool RectangleComponent::SetAttribute(int32_t type, uint64_t val)
    {
        if (type == 10)
        {
            fillColor = *((uint32_t*)&val);
            return true;
        }

        return false;
    }

    uint64_t RectangleComponent::GetAttribute(int32_t type)
    {
        uint8_t temp[8];
        for (int i = 0; i < 8; i++)
            temp[i] = 0;

        if (type == 10)
            *((uint32_t*)temp) = fillColor;

        return *((uint64_t*)temp);
    }

    int RectangleComponent::GetAttributeSize(int32_t type)
    {
        if (type == 10)
            return 4;

        return 0;
    }

}