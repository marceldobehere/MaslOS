#include "screenComponent.h"
#include "../../../../../../Rendering/Cols.h"
#include "../../../../../../OSDATA/MStack/MStackM.h"

namespace GuiComponentStuff
{
    ScreenComponent::ScreenComponent(Window* window)
    {
        this->window = window;
        size = ComponentSize((int)window->framebuffer->Width, (int)window->framebuffer->Height);
        oldSize = size;
        componentType = SCREEN;
        parent = NULL;
        children = new List<BaseComponent*>(5);
        renderer = new ComponentRenderer(size);
        renderer->bgCol = Colors.bgray;
        renderer->Fill(Colors.bgray);

        tempSelectedComponent = NULL;
        selectedComponent = NULL;
    }

    void ScreenComponent::Render(Field field)
    {
        AddToStack();
        tempSelectedComponent = NULL;
        ComponentSize tSize = ComponentSize((int)window->framebuffer->Width, (int)window->framebuffer->Height);

        if (oldSize != tSize)
        {
            renderer->Resize(tSize);
            size = tSize;
            oldSize = tSize;
        }

        {
            Field temp = field - position;
            renderer->Fill(renderer->bgCol, temp);
        }

        Field bruh = field - position;

        for (int i = 0; i < children->getCount(); i++)
        {
            children->elementAt(i)->Render(bruh);
        }
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

    void ScreenComponent::Destroy(bool destroyChildren)
    {
        renderer->Free();
        if (destroyChildren)
            for (int i = 0; i < children->getCount(); i++)
                children->elementAt(i)->Destroy(true);
        children->free();
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
}