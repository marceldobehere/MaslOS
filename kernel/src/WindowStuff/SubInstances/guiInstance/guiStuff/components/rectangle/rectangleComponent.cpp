#include "rectangleComponent.h"


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
    }

    void RectangleComponent::MouseClicked(Position mousePos)
    {

    }

    void RectangleComponent::Render(Field field)
    {
        ComponentSize temp = GetActualComponentSize();
        if (oldSize != temp)
        {
            renderer->Resize(temp);
            renderer->Fill(fillColor);
            oldSize = temp;
        }

        renderer->Render(position, field, parent->renderer->componentFrameBuffer);
    }

    void RectangleComponent::Destroy(bool destroyChildren)
    {
        renderer->Free();
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

}