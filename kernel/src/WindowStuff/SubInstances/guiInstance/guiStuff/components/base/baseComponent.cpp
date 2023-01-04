#include "baseComponent.h"
#include "../../../../../../customClasses/list.h"
#include "../rectangle/rectangleComponent.h"
#include "../screenComponent/screenComponent.h"
#include "../box/boxComponent.h"
#include "../text/textComponent.h"
#include "../button/buttonComponent.h"

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
        if (componentType == BUTTON)
            ((ButtonComponent*)this)->Render(field);
    }

    void BaseComponent::MouseClicked(MouseClickEventInfo info)
    {
        if (componentType == RECT)
            ((RectangleComponent*)this)->MouseClicked(info);
        if (componentType == SCREEN)
            ((ScreenComponent*)this)->MouseClicked(info);
        if (componentType == BOX)
            ((BoxComponent*)this)->MouseClicked(info);
        if (componentType == TEXT)
            ((TextComponent*)this)->MouseClicked(info);
        if (componentType == BUTTON)
            ((ButtonComponent*)this)->MouseClicked(info);
    }

    void BaseComponent::KeyHit(KeyHitEventInfo info)
    {

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
        if (componentType == BUTTON)
            ((ButtonComponent*)this)->Destroy(destroyChildren);
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
        if (componentType == BUTTON)
            return ((ButtonComponent*)this)->GetActualComponentSize();

        return ComponentSize(0, 0);
    }

    Position BaseComponent::GetAbsoluteComponentPosition()
    {
        if (componentType == ComponentType::SCREEN || parent == NULL)
            return this->position;
        else
            return this->position + parent->GetAbsoluteComponentPosition();
    }

    void* BaseComponent::GetWindow()
    {
        if (componentType == ComponentType::SCREEN)
            return (void*)((ScreenComponent*)this)->window;
        else if (parent == NULL)
            return NULL;
        else
            return parent->GetWindow();
    }

    void* BaseComponent::GetScreen()
    {
        if (componentType == ComponentType::SCREEN)
            return (void*)this;
        else if (parent == NULL)
            return NULL;
        else
            return parent->GetScreen();
    }

}