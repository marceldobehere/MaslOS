#include "screenComponent.h"
#include "../../../../../../rendering/Cols.h"
#include "../../../../../../osData/MStack/MStackM.h"

namespace GuiComponentStuff
{
    ScreenComponent::ScreenComponent(Window* window)
    {
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

        updateFields->add(Field(Position(), GetActualComponentSize()));
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

        for (int i = 0; i < children->getCount(); i++)
            children->elementAt(i)->CheckUpdates();
        {
            int cCount = children->getCount();
            while (childrenFields->getCount() > cCount)
            {
                updateFields->add(childrenFields->elementAt(childrenFields->getCount() - 1));
                childrenFields->removeLast();
                childrenHidden->removeLast();
            }
            while (childrenFields->getCount() < cCount)
            {
                childrenFields->add(Field());
                childrenHidden->add(false);
            }
        }
        {
            for (int i = 0; i < childrenFields->getCount(); i++)
            {
                Field a = childrenFields->elementAt(i);
                Field b = children->elementAt(i)->GetFieldWithPos();
                bool a1 = childrenHidden->elementAt(i);
                bool b1 = children->elementAt(i)->hidden;

                if (a != b || a1 != b1)
                {
                    childrenFields->set(i, b);
                    childrenHidden->set(i, b1);
                    updateFields->add(a);
                    updateFields->add(b);
                }
            }
        }
        {
            for (int i = 0; i < childrenFields->getCount(); i++)
            {
                Field a = childrenFields->elementAt(i);
                Field b = children->elementAt(i)->GetFieldWithPos();

                if (a != b)
                {
                    childrenFields->set(i, b);
                    updateFields->add(a);
                    updateFields->add(b);
                }
            }
        }
        if (update)
        {
            updateFields->clear();
            updateFields->add(Field(Position(), GetActualComponentSize()));
        }

        AddToStack();
        while (updateFields->getCount() > 0)
        {
            Field bruh = updateFields->elementAt(0);
            updateFields->removeAt(0);
            finalUpdatedFields->add(bruh);
            renderer->bgCol = backgroundColor;
            renderer->Fill(renderer->bgCol, bruh);
            for (int i = 0; i < children->getCount(); i++)
                children->elementAt(i)->Render(bruh - children->elementAt(i)->position);

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
            for (int i = 0; i < children->getCount(); i++)
            {
                AddToStack();
                BaseComponent* ch = children->elementAt(i);
                ch->Destroy(true, callBackFunc);
                _Free(ch);
                RemoveFromStack();
            }
        AddToStack();
        children->free();
        _Free(children);
        updateFields->free();
        _Free(updateFields);
        finalUpdatedFields->free();
        _Free(finalUpdatedFields);
        childrenFields->free();
        _Free(childrenFields);
        childrenHidden->free();
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