#include "boxComponent.h"
#include "../../../../../../OSDATA/MStack/MStackM.h"

namespace GuiComponentStuff
{
    // BoxComponent::BoxComponent(BaseComponent* parent)
    // {
    //     this->parent = parent;
    //     this->size = ComponentSize(true);
    //     oldSize = size;
    //     componentType = ComponentType::BOX;
    //     children = new List<BaseComponent*>(5);

    //     ComponentSize temp = GetActualComponentSize(this);

    //     renderer = new ComponentRenderer(temp);
    //     renderer->bgCol = backgroundColor;
    //     renderer->Fill(backgroundColor);
    // }

    BoxComponent::BoxComponent(BaseComponent* parent, ComponentSize size, uint32_t bgCol)
    {
        this->parent = parent;
        this->size = size;
        oldSize = size;
        backgroundColor = bgCol;
        componentType = ComponentType::BOX;
        children = new List<BaseComponent*>(5);

        ComponentSize temp = GetActualComponentSize();

        renderer = new ComponentRenderer(temp);
        renderer->bgCol = backgroundColor;
        renderer->Fill(backgroundColor);
    }

    void BoxComponent::MouseClicked(MouseClickEventInfo info)
    {

    }

    void BoxComponent::KeyHit(KeyHitEventInfo info)
    {

    }
    
    void BoxComponent::Render(Field field)
    {
        AddToStack();
        ComponentSize temp = GetActualComponentSize();
        if (oldSize != temp)
        {
            renderer->Resize(temp);
            renderer->Fill(backgroundColor);
            oldSize = temp;
        }

        Field bruh = field - position;

        for (int i = 0; i < children->getCount(); i++)
        {
            children->elementAt(i)->Render(bruh);
        }

        renderer->Render(position, field, parent->renderer->componentFrameBuffer);
        RemoveFromStack();
    }

    void BoxComponent::Destroy(bool destroyChildren, void (*callBackFunc)(BaseComponent* comp))
    {
        renderer->Free();
        if (destroyChildren)
            for (int i = 0; i < children->getCount(); i++)
            {
                BaseComponent* ch = children->elementAt(i);
                callBackFunc(ch);
                ch->Destroy(true, callBackFunc);
                _Free((void*)ch);
            }
        children->free();
        _Free(children);
    }

    ComponentSize BoxComponent::GetActualComponentSize()
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


    bool BoxComponent::SetAttribute(int32_t type, uint64_t val)
    {
        if (type == 10)
        {
            backgroundColor= *((uint32_t*)&val);
            return true;
        }

        return false;
    }

    uint64_t BoxComponent::GetAttribute(int32_t type)
    {
        uint8_t temp[8];
        for (int i = 0; i < 8; i++)
            temp[i] = 0;

        if (type == 10)
            *((uint32_t*)temp) = backgroundColor;

        return *((uint64_t*)temp);
    }

    int BoxComponent::GetAttributeSize(int32_t type)
    {
        if (type == 10)
            return 4;

        return 0;
    }
}