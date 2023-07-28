#include "screenComponent.h"
#include "../../../../../../rendering/Cols.h"
#include "../../../../../../osData/MStack/MStackM.h"

namespace GuiComponentStuff
{
    ScreenComponent::ScreenComponent(Window* window)
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

        this->window = window;
        updateFields = new List<Field>(5);
        finalUpdatedFields = new List<Field>(5);
        size = ComponentSize((int)window->framebuffer->Width, (int)window->framebuffer->Height);
        oldSize = size;
        componentType = SCREEN;
        parent = NULL;
        children = new List<BaseComponent*>(5);
        childrenFields = new List<Field>(5);
        childrenHidden = new List<bool>(5);
        renderer = new ComponentRenderer(size);
        backgroundColor = Colors.bgray;
        renderer->bgCol = backgroundColor;
        renderer->Fill(Colors.bgray);

        tempSelectedComponent = NULL;
        selectedComponent = NULL;

        updateFields->Add(Field(Position(), GetActualComponentSize()));
        CheckUpdates(); 
    }

    void ScreenComponent::CheckUpdates()
    {
        AddToStack();


        tempSelectedComponent = NULL;
        ComponentSize tSize = ComponentSize((int)window->framebuffer->Width, (int)window->framebuffer->Height);
        bool update = false;


        AddToStack();
        if (oldSize != tSize)
        {
            renderer->Resize(tSize);
            size = tSize;
            oldSize = tSize;
            update = true;
        }
        RemoveFromStack();

        AddToStack();
        if (backgroundColor != renderer->bgCol)
        {
            renderer->bgCol = backgroundColor;
            renderer->Fill(backgroundColor);
            update = true;
        }
        RemoveFromStack();

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
                childrenHidden->Add(false);
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
        {
            for (int i = 0; i < childrenFields->GetCount(); i++)
            {
                Field a = childrenFields->ElementAt(i);
                Field b = children->ElementAt(i)->GetFieldWithPos();

                if (a != b)
                {
                    childrenFields->Set(i, b);
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
        while (updateFields->GetCount() > 0)
        {
            Field bruh = updateFields->LastElement();
            updateFields->RemoveLast();
            finalUpdatedFields->Add(bruh);
            renderer->bgCol = backgroundColor;
            renderer->Fill(renderer->bgCol, bruh);
            for (int i = 0; i < children->GetCount(); i++)
                children->ElementAt(i)->Render(bruh - children->ElementAt(i)->position);

        }
        RemoveFromStack();



        RemoveFromStack();
    }

    void ScreenComponent::Render(Field field)
    {
        AddToStack();
        // tempSelectedComponent = NULL;
        // ComponentSize tSize = ComponentSize((int)window->framebuffer->Width, (int)window->framebuffer->Height);

        // AddToStack();
        // if (oldSize != tSize)
        // {
        //     renderer->Resize(tSize);
        //     size = tSize;
        //     oldSize = tSize;
        // }
        // RemoveFromStack();

        // AddToStack();
        // {
        //     Field temp = field - position;
        //     renderer->bgCol = backgroundColor;
        //     renderer->Fill(renderer->bgCol, temp);
        // }
        // RemoveFromStack();

        // Field bruh = field - position;

        // AddToStack();
        // for (int i = 0; i < children->getCount(); i++)
        // {
        //     children->elementAt(i)->Render(bruh);
        // }
        // RemoveFromStack();

        RemoveFromStack();
    }

    ComponentSize ScreenComponent::GetActualComponentSize()
    {
        return size;
    }

    void ScreenComponent::MouseClicked(MouseClickEventInfo info)
    {
        AddToStack();
        selectedComponent = tempSelectedComponent;
        if (selectedComponent != NULL)
            selectedComponent->MouseClicked(info);
        RemoveFromStack();
    }

    void ScreenComponent::KeyHit(KeyHitEventInfo info)
    {
        AddToStack();
        //selectedComponent = tempSelectedComponent;
        if (selectedComponent != NULL)
            selectedComponent->KeyHit(info);
        RemoveFromStack();
    }

    void ScreenComponent::Destroy(bool destroyChildren, void (*callBackFunc)(BaseComponent* comp))
    {
        AddToStack();
        if (callBackFunc != NULL)
            callBackFunc(this);
        if (destroyChildren)
            for (int i = 0; i < children->GetCount(); i++)
            {
                AddToStack();
                BaseComponent* ch = children->ElementAt(i);
                ch->Destroy(true, callBackFunc);
                _Free(ch);
                RemoveFromStack();
            }
        AddToStack();
        children->Free();
        _Free(children);
        updateFields->Free();
        _Free(updateFields);
        finalUpdatedFields->Free();
        _Free(finalUpdatedFields);
        childrenFields->Free();
        _Free(childrenFields);
        childrenHidden->Free();
        _Free(childrenHidden);
        RemoveFromStack();

        AddToStack();
        renderer->Free();
        RemoveFromStack();
        
        RemoveFromStack();
    }
    


    bool ScreenComponent::SetAttribute(int32_t type, uint64_t val)
    {
        if (type == 11)
        {
            backgroundColor = *((uint32_t*)&val);
            return true;
        }


        return false;
    }

    uint64_t ScreenComponent::GetAttribute(int32_t type)
    {
        uint8_t temp[8];
        for (int i = 0; i < 8; i++)
            temp[i] = 0;

        if (type == 10)
            *((int64_t*)temp) = id;
        else if (type == 11)
            *((uint32_t*)temp) = backgroundColor;
        else if (type == 12)
        {
            if (selectedComponent != NULL)
                *((int64_t*)temp) = 0;
            else
                *((int64_t*)temp) = selectedComponent->id;
        }

        return *((uint64_t*)temp);
    }

    int ScreenComponent::GetAttributeSize(int32_t type)
    {
        if (type == 10)
            return 8;
        if (type == 11)
            return 4;
        if (type == 12)
            return 8;

        return 0;
    }
}