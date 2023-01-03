#include "baseComponent.h"
#include "../../../../../../customClasses/list.h"
#include "../rectangle/rectangleComponent.h"
#include "../screenComponent/screenComponent.h"
#include "../box/boxComponent.h"
#include "../text/textComponent.h"

namespace GuiComponentStuff
{

    BaseComponent::BaseComponent(BaseComponent* parent)
    {
        this->parent = parent;
    }

    BaseComponent::BaseComponent()
    {
        this->parent = NULL;
    }

    void BaseComponent::Render(Field field)
    {
        if (hidden)
            return;

        if (componentType == RECT)
            ((RectangleComponent*)this)->Render(field);
        if (componentType == SCREEN)
            ((ScreenComponent*)this)->Render(field);
        if (componentType == BOX)
            ((BoxComponent*)this)->Render(field);
        if (componentType == TEXT)
            ((TextComponent*)this)->Render(field);
    }

    void BaseComponent::MouseClicked(Position mousePos)
    {
        if (componentType == RECT)
            ((RectangleComponent*)this)->MouseClicked(mousePos);
        if (componentType == SCREEN)
            ((ScreenComponent*)this)->MouseClicked(mousePos);
        if (componentType == BOX)
            ((BoxComponent*)this)->MouseClicked(mousePos);
        if (componentType == TEXT)
            ((TextComponent*)this)->MouseClicked(mousePos);
    }

    void BaseComponent::Destroy(bool destroyChildren)
    {
        if (componentType == RECT)
            ((RectangleComponent*)this)->Destroy(destroyChildren);
        if (componentType == SCREEN)
            ((ScreenComponent*)this)->Destroy(destroyChildren);
        if (componentType == BOX)
            ((BoxComponent*)this)->Destroy(destroyChildren);
        if (componentType == TEXT)
            ((TextComponent*)this)->Destroy(destroyChildren);
    }

    ComponentSize BaseComponent::GetActualComponentSize()
    {
        if (componentType == RECT)
            return ((RectangleComponent*)this)->GetActualComponentSize();
        if (componentType == SCREEN)
            return ((ScreenComponent*)this)->GetActualComponentSize();
        if (componentType == BOX)
            return ((BoxComponent*)this)->GetActualComponentSize();
        if (componentType == TEXT)
            return ((TextComponent*)this)->GetActualComponentSize();

        return ComponentSize(0, 0);
    }


}