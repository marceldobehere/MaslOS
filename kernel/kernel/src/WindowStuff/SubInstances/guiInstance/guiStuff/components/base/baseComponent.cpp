#include "baseComponent.h"

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
        if (RenderFunc != NULL)
            RenderFunc(this, field);
        else
            Panic("COMPONENT HAS NO RENDER FUNC!", true);
        RemoveFromStack();
    }

    void BaseComponent::CheckUpdates()
    {
        if (hidden)
            return;

        AddToStack();
        if (CheckUpdatesFunc != NULL)
            CheckUpdatesFunc(this);
        else
            Panic("COMPONENT HAS NO CHECK UPDATES FUNC!", true);
        RemoveFromStack();
    }

    void BaseComponent::MouseClicked(MouseClickEventInfo info)
    {
        AddToStack();
        if (MouseClickedFunc != NULL)
            MouseClickedFunc(this, info);
        else
            Panic("COMPONENT HAS NO MOUSE CLICKED FUNC!", true);
        RemoveFromStack();
    }

    void BaseComponent::KeyHit(KeyHitEventInfo info)
    {
        AddToStack();
        if (KeyHitFunc != NULL)
            KeyHitFunc(this, info);
        else
            Panic("COMPONENT HAS NO KEY HIT FUNC!", true);
        RemoveFromStack();
    }

    bool BaseComponent::Destroy(bool destroyChildren, void (*callBackFunc)(BaseComponent* comp))
    {
        AddToStack();
        if (parent != NULL)
        {
            GuiInstance* blehus = ((GuiInstance*)((Window*)GetWindow())->instance);
            int indx = blehus->GetIndexOfChildFromComponentWithId(parent->id, id);
            blehus->RemoveChildFromComponentWithId(parent->id, indx);
        }
        
        if (DestroyFunc != NULL)
            DestroyFunc(this, destroyChildren, callBackFunc);
        else
            Panic("COMPONENT HAS NO DESTROY FUNC!", true);

        RemoveFromStack();
        return true;
    }

    ComponentSize BaseComponent::GetActualComponentSize()
    {
        AddToStack();
        if (GetActualComponentSizeFunc != NULL)
        {
            ComponentSize size = GetActualComponentSizeFunc(this);
            RemoveFromStack();
            if (size.FixedX < 0)
                size.FixedX = 0;
            if (size.FixedY < 0)
                size.FixedY = 0;
            return size;
        }
        else
            Panic("COMPONENT HAS NO GET ACTUAL COMPONENT SIZE FUNC!", true);
        RemoveFromStack();
        return ComponentSize();
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
        AddToStack();
        if (SetAttributeFunc != NULL)
        {
            bool ret = SetAttributeFunc(this, type, val);
            RemoveFromStack();
            return ret;
        }
        else
            Panic("COMPONENT HAS NO SET ATTRIBUTE FUNC!", true);
        RemoveFromStack();
        return false;
    }

    uint64_t BaseComponent::GetAttribute(int32_t type)
    {
        AddToStack();
        if (GetAttributeFunc != NULL)
        {
            uint64_t ret = GetAttributeFunc(this, type);
            RemoveFromStack();
            return ret;
        }
        else
            Panic("COMPONENT HAS NO GET ATTRIBUTE FUNC!", true);
        RemoveFromStack();
        return 0;
    }

    int BaseComponent::GetAttributeSize(int32_t type)
    {
        AddToStack();
        if (GetAttributeSizeFunc != NULL)
        {
            int ret = GetAttributeSizeFunc(this, type);
            RemoveFromStack();
            return ret;
        }
        else
            Panic("COMPONENT HAS NO GET ATTRIBUTE SIZE FUNC!", true);
        RemoveFromStack();
        return 0;
    }

}