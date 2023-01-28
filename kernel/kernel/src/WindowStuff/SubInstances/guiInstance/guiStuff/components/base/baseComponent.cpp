#include "baseComponent.h"
#include "../../../../../../customClasses/list.h"
#include "../rectangle/rectangleComponent.h"
#include "../screenComponent/screenComponent.h"
#include "../box/boxComponent.h"
#include "../text/textComponent.h"
#include "../button/buttonComponent.h"
#include "../textField/textFieldComponent.h"
#include "../../../../../../OSDATA/MStack/MStackM.h"
#include "../../../guiInstance.h"

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

        AddToStack();
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
        if (componentType == TEXTFIELD)
            ((TextFieldComponent*)this)->Render(field);
        RemoveFromStack();
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
        if (componentType == TEXTFIELD)
            ((TextFieldComponent*)this)->MouseClicked(info);
    }

    void BaseComponent::KeyHit(KeyHitEventInfo info)
    {
        if (componentType == RECT)
            ((RectangleComponent*)this)->KeyHit(info);
        if (componentType == SCREEN)
            ((ScreenComponent*)this)->KeyHit(info);
        if (componentType == BOX)
            ((BoxComponent*)this)->KeyHit(info);
        if (componentType == TEXT)
            ((TextComponent*)this)->KeyHit(info);
        if (componentType == BUTTON)
            ((ButtonComponent*)this)->KeyHit(info);
        if (componentType == TEXTFIELD)
            ((TextFieldComponent*)this)->KeyHit(info);
    }

    bool BaseComponent::Destroy(bool destroyChildren, void (*callBackFunc)(BaseComponent* comp))
    {
        if (componentType == RECT)
            ((RectangleComponent*)this)->Destroy(destroyChildren, callBackFunc);
        if (componentType == SCREEN)
            ((ScreenComponent*)this)->Destroy(destroyChildren, callBackFunc);
        if (componentType == BOX)
            ((BoxComponent*)this)->Destroy(destroyChildren, callBackFunc);
        if (componentType == TEXT)
            ((TextComponent*)this)->Destroy(destroyChildren, callBackFunc);
        if (componentType == BUTTON)
            ((ButtonComponent*)this)->Destroy(destroyChildren, callBackFunc);
        if (componentType == TEXTFIELD)
            ((TextFieldComponent*)this)->Destroy(destroyChildren, callBackFunc);

        if (parent != NULL)
        {
            int indx = ((GuiInstance*)((Window*)GetWindow())->instance)->GetIndexOfChildFromComponentWithId(parent->id, id);
            return ((GuiInstance*)((Window*)GetWindow())->instance)->RemoveChildFromComponentWithId(parent->id, indx);
        }

        return true;
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
        if (componentType == TEXTFIELD)
            return ((TextFieldComponent*)this)->GetActualComponentSize();

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

    bool BaseComponent::IsVisible()
    {
        if (componentType == ComponentType::SCREEN)
            return !hidden;
        else if (parent == NULL)
            return false;
        else
            return !hidden && parent->IsVisible();
    }

    // BaseComponent* BaseComponent::GetComponentFromId(uint64_t id)
    // {
    //     if (this->id == id)
    //         return this;

    //     if (componentType == ComponentType::SCREEN)
    //     {
    //         BaseComponent* found = NULL;
    //         ScreenComponent* screen = (ScreenComponent*)this;
    //         for (int i = 0; found == NULL && i < screen->children->getCount(); i++)
    //             found = screen->children->elementAt(i)->GetComponentFromId(id);
                
    //         return found;
    //     }

    //     if (componentType == ComponentType::BOX)
    //     {
    //         BaseComponent* found = NULL;
    //         BoxComponent* box = (BoxComponent*)this;
    //         for (int i = 0; found == NULL && i < box->children->getCount(); i++)
    //             found = box->children->elementAt(i)->GetComponentFromId(id);
                
    //         return found;
    //     }

    //     if (componentType == ComponentType::BUTTON)
    //         return ((ButtonComponent*)this)->actualButtonStuff->GetComponentFromId(id);

    //     if (componentType == ComponentType::TEXTFIELD)
    //         return ((TextFieldComponent*)this)->actualButtonStuff->GetComponentFromId(id);

    //     return NULL;
    // }

    void* BaseComponent::GetScreen()
    {
        if (componentType == ComponentType::SCREEN)
            return (void*)this;
        else if (parent == NULL)
            return NULL;
        else
            return parent->GetScreen();
    }


    bool BaseComponent::SetAttribute(int32_t type, uint64_t val)
    {
        if (componentType == RECT)
            return ((RectangleComponent*)this)->SetAttribute(type, val);
        if (componentType == SCREEN)
            return ((ScreenComponent*)this)->SetAttribute(type, val);
        if (componentType == BOX)
            return ((BoxComponent*)this)->SetAttribute(type, val);
        if (componentType == TEXT)
            return ((TextComponent*)this)->SetAttribute(type, val);
        if (componentType == BUTTON)
            return ((ButtonComponent*)this)->SetAttribute(type, val);
        if (componentType == TEXTFIELD)
            return ((TextFieldComponent*)this)->SetAttribute(type, val);

        return false;
    }

    uint64_t BaseComponent::GetAttribute(int32_t type)
    {
        if (componentType == RECT)
            return ((RectangleComponent*)this)->GetAttribute(type);
        if (componentType == SCREEN)
            return ((ScreenComponent*)this)->GetAttribute(type);
        if (componentType == BOX)
            return ((BoxComponent*)this)->GetAttribute(type);
        if (componentType == TEXT)
            return ((TextComponent*)this)->GetAttribute(type);
        if (componentType == BUTTON)
            return ((ButtonComponent*)this)->GetAttribute(type);
        if (componentType == TEXTFIELD)
            return ((TextFieldComponent*)this)->GetAttribute(type);

        return 0;
    }

    int BaseComponent::GetAttributeSize(int32_t type)
    {
        if (componentType == RECT)
            return ((RectangleComponent*)this)->GetAttributeSize(type);
        if (componentType == SCREEN)
            return ((ScreenComponent*)this)->GetAttributeSize(type);
        if (componentType == BOX)
            return ((BoxComponent*)this)->GetAttributeSize(type);
        if (componentType == TEXT)
            return ((TextComponent*)this)->GetAttributeSize(type);
        if (componentType == BUTTON)
            return ((ButtonComponent*)this)->GetAttributeSize(type);
        if (componentType == TEXTFIELD)
            return ((TextFieldComponent*)this)->GetAttributeSize(type);

        return 0;
    }

}