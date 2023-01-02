#include "rectangleComponent.h"


namespace GuiComponentStuff
{
    RectangleComponent::RectangleComponent(uint32_t fillColor, ComponentSize size, BaseComponent* parent)
    {
        this->fillColor = fillColor;
        wantedSize = size;
        componentType = RECT;
        this->parent = parent;
        actualSize = GetActualComponentSize();
        renderer = new ComponentRenderer(actualSize);
        renderer->bgCol = fillColor;
        renderer->Fill(fillColor);
    }

    void RectangleComponent::MouseClicked(Position mousePos)
    {

    }

    void RectangleComponent::Render(Field field)
    {
        ComponentSize temp = GetActualComponentSize();
        if (actualSize.FixedX != temp.FixedX || actualSize.FixedY != temp.FixedY)
        {
            renderer->Resize(temp);
            renderer->Fill(fillColor);
            actualSize = temp;
        }

        renderer->Render(position, field, parent->renderer->componentFrameBuffer);
    }

    void RectangleComponent::Destroy(bool destroyChildren)
    {
        renderer->Free();
    }

    ComponentSize RectangleComponent::GetActualComponentSize()
    {
        if (wantedSize.IsXFixed && wantedSize.IsYFixed)
            return ComponentSize(wantedSize);

        ComponentSize temp = ComponentSize(0, 0);
        if (wantedSize.IsXFixed)
            temp.FixedX = wantedSize.FixedX;
        else
            temp.FixedX = wantedSize.ScaledX * parent->GetActualComponentSize().FixedX;

        if (wantedSize.IsYFixed)
            temp.FixedY = wantedSize.FixedY;
         else
            temp.FixedY = wantedSize.ScaledY * parent->GetActualComponentSize().FixedY;


        return temp;
    }

}