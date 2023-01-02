#include "baseComponent.h"
#include "../../../../../../customClasses/list.h"
#include "../rectangle/rectangleComponent.h"
#include "../screenComponent/screenComponent.h"

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
        if (componentType == RECT)
            ((RectangleComponent*)this)->Render(field);
        if (componentType == SCREEN)
            ((ScreenComponent*)this)->Render(field);
    }

    void BaseComponent::MouseClicked(Position mousePos)
    {
        if (componentType == RECT)
            ((RectangleComponent*)this)->MouseClicked(mousePos);
        if (componentType == SCREEN)
            ((ScreenComponent*)this)->MouseClicked(mousePos);
    }

    void BaseComponent::Destroy(bool destroyChildren)
    {
        if (componentType == RECT)
            ((RectangleComponent*)this)->Destroy(destroyChildren);
        if (componentType == SCREEN)
            ((ScreenComponent*)this)->Destroy(destroyChildren);
    }

    ComponentSize BaseComponent::GetActualComponentSize()
    {
        if (componentType == RECT)
            return ((RectangleComponent*)this)->GetActualComponentSize();
        if (componentType == SCREEN)
            return ((ScreenComponent*)this)->GetActualComponentSize();

        return ComponentSize(0, 0);
    }


}