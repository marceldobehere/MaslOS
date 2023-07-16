#include "baseComponent.h"
#include "../../../../../../cStdLib/list.h"

#include "../rectangle/rectangleComponent.h"
#include "../screenComponent/screenComponent.h"
#include "../box/boxComponent.h"
#include "../text/textComponent.h"
#include "../button/buttonComponent.h"
#include "../textField/textFieldComponent.h"
#include "../imageRect/imageRectangleComponent.h"
#include "../canvas/canvasComponent.h"

#include "../../../../../../osData/MStack/MStackM.h"
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
        if (componentType == IMAGE_RECT)
            ((ImageRectangleComponent*)this)->Render(field);
        if (componentType == CANVAS)
            ((CanvasComponent*)this)->Render(field);
        RemoveFromStack();
    }

    void BaseComponent::CheckUpdates()
    {
        if (hidden)
            return;

        AddToStack();
        if (componentType == RECT)
            ((RectangleComponent*)this)->CheckUpdates();
        if (componentType == SCREEN)
            ((ScreenComponent*)this)->CheckUpdates();
        if (componentType == BOX)
            ((BoxComponent*)this)->CheckUpdates();
        if (componentType == TEXT)
            ((TextComponent*)this)->CheckUpdates();
        if (componentType == BUTTON)
            ((ButtonComponent*)this)->CheckUpdates();
        if (componentType == TEXTFIELD)
            ((TextFieldComponent*)this)->CheckUpdates();
        if (componentType == IMAGE_RECT)
            ((ImageRectangleComponent*)this)->CheckUpdates();
        if (componentType == CANVAS)
            ((CanvasComponent*)this)->CheckUpdates();
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
        if (componentType == IMAGE_RECT)
            ((ImageRectangleComponent*)this)->MouseClicked(info);
        if (componentType == CANVAS)
            ((CanvasComponent*)this)->MouseClicked(info);
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
        if (componentType == IMAGE_RECT)
            ((ImageRectangleComponent*)this)->KeyHit(info);
        if (componentType == CANVAS)
            ((CanvasComponent*)this)->KeyHit(info);
    }

    bool BaseComponent::Destroy(bool destroyChildren, void (*callBackFunc)(BaseComponent* comp))
    {
        AddToStack();
        if (parent != NULL)
        {
            GuiInstance* blehus = ((GuiInstance*)((Window*)GetWindow())->instance);
            int indx = blehus->GetIndexOfChildFromComponentWithId(parent->id, id);
            // GlobalRenderer->Clear(Colors.dgray);
            // GlobalRenderer->Println("P: {}", to_string(parent->id), Colors.white);
            // GlobalRenderer->Println("C: {}", to_string(id), Colors.white);
            // GlobalRenderer->Println("TEST: {}", to_string(indx), Colors.white);
            // while (true);
            blehus->RemoveChildFromComponentWithId(parent->id, indx);
        }
        
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
        if (componentType == IMAGE_RECT)
            ((ImageRectangleComponent*)this)->Destroy(destroyChildren, callBackFunc);
        if (componentType == CANVAS)
            ((CanvasComponent*)this)->Destroy(destroyChildren, callBackFunc);

        RemoveFromStack();
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
        if (componentType == IMAGE_RECT)
            return ((ImageRectangleComponent*)this)->GetActualComponentSize();
        if (componentType == CANVAS)
            return ((CanvasComponent*)this)->GetActualComponentSize();

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

    Field BaseComponent::GetFieldWithPos()
    {
        return Field(position, GetActualComponentSize());
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
        if (componentType == IMAGE_RECT)
            return ((ImageRectangleComponent*)this)->SetAttribute(type, val);
        if (componentType == CANVAS)
            return ((CanvasComponent*)this)->SetAttribute(type, val);

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
        if (componentType == IMAGE_RECT)
            return ((ImageRectangleComponent*)this)->GetAttribute(type);
        if (componentType == CANVAS)
            return ((CanvasComponent*)this)->GetAttribute(type);

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
        if (componentType == IMAGE_RECT)
            return ((ImageRectangleComponent*)this)->GetAttributeSize(type);
        if (componentType == CANVAS)
            return ((CanvasComponent*)this)->GetAttributeSize(type);

        return 0;
    }

}