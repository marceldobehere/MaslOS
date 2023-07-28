#include "boxComponent.h"
#include "../../../../../../osData/MStack/MStackM.h"

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
        RenderFunc = (void (*)(void*, Field))&Render;
        CheckUpdatesFunc = (void (*)(void*))&CheckUpdates;
        MouseClickedFunc = (void (*)(void*, MouseClickEventInfo))&MouseClicked;
        KeyHitFunc = (void (*)(void*, KeyHitEventInfo))&KeyHit;
        DestroyFunc = (void (*)(void*, bool, void (*)(BaseComponent* comp)))&Destroy;
        GetActualComponentSizeFunc = (ComponentSize (*)(void*))&GetActualComponentSize;
        SetAttributeFunc = (bool (*)(void*, int32_t, uint64_t))&SetAttribute;
        GetAttributeFunc = (uint64_t (*)(void*, int32_t))&GetAttribute;
        GetAttributeSizeFunc = (int (*)(void*, int32_t))&GetAttributeSize;

        this->parent = parent;
        this->size = size;
        updateFields = new List<Field>(5);
        oldSize = size;
        backgroundColor = bgCol;
        componentType = ComponentType::BOX;
        children = new List<BaseComponent*>(5);
        childrenFields = new List<Field>(5);
        childrenHidden = new List<bool>(5);

        ComponentSize temp = GetActualComponentSize();

        renderer = new ComponentRenderer(temp);
        renderer->bgCol = backgroundColor;
        renderer->Fill(backgroundColor);

        CheckUpdates();
        //Render(Field(Position(), GetActualComponentSize()));
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
        if (!hidden)
            renderer->Render(position, field, parent->renderer->componentFrameBuffer);
        RemoveFromStack();
    }

    void BoxComponent::CheckUpdates()
    {
        AddToStack();
        bool update = false;

        ComponentSize temp = GetActualComponentSize();
        if (oldSize != temp)
        {
            renderer->Resize(temp);
            
            oldSize = temp;

            renderer->Fill(backgroundColor);
            update = true;
        }

        for (int i = 0; i < children->GetCount(); i++)
            children->ElementAt(i)->CheckUpdates();
        {
            int cCount = children->GetCount();
            while (childrenFields->GetCount() > cCount)
            {
                updateFields->Add(childrenFields->ElementAt(childrenFields->GetCount() - 1));
                childrenFields->RemoveLast();
                childrenHidden->RemoveLast();
            }
            while (childrenFields->GetCount() < cCount)
            {
                childrenFields->Add(Field());
                childrenHidden->Add(true);
            }
        }
        {
            for (int i = 0; i < childrenFields->GetCount(); i++)
            {
                Field a = childrenFields->ElementAt(i);
                Field b = children->ElementAt(i)->GetFieldWithPos();
                bool a1 = childrenHidden->ElementAt(i);
                bool b1 = children->ElementAt(i)->hidden;

                if (a != b || a1 != b1)
                {
                    childrenFields->Set(i, b);
                    childrenHidden->Set(i, b1);
                    updateFields->Add(a);
                    updateFields->Add(b);
                }
            }
        }
        if (update)
        {
            updateFields->Clear();
            updateFields->Add(Field(Position(), GetActualComponentSize()));
        }


        AddToStack();
        // if (updateFields->getCount() > 0)
        //     update = true;
        while (updateFields->GetCount() > 0)
        {
            Field bruh = updateFields->ElementAt(0);
            updateFields->RemoveAt(0);

            renderer->Fill(backgroundColor, bruh);

            for (int i = 0; i < children->GetCount(); i++)
            {
                children->ElementAt(i)->Render(bruh - children->ElementAt(i)->position);
            }

            parent->updateFields->Add(bruh + position);
        }
        RemoveFromStack();
        

        RemoveFromStack();
    }

    void BoxComponent::Destroy(bool destroyChildren, void (*callBackFunc)(BaseComponent* comp))
    {
        AddToStack();
        if (callBackFunc != NULL)
            callBackFunc(this);
        if (destroyChildren)
            for (int i = 0; i < children->GetCount(); i++)
            {
                BaseComponent* ch = children->ElementAt(i);
                ch->Destroy(true, callBackFunc);
                _Free((void*)ch);
            }
        children->Free();
        _Free(children);
        renderer->Free();
        updateFields->Free();
        _Free(updateFields);
        childrenFields->Free();
        _Free(childrenFields);
        childrenHidden->Free();
        _Free(childrenHidden);
        RemoveFromStack();
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

        if (temp.FixedX < 0)
            temp.FixedX = 0;
        if (temp.FixedY < 0)
            temp.FixedY = 0;

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