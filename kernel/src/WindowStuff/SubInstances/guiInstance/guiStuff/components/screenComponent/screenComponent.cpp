#include "screenComponent.h"
#include "../../../../../../Rendering/Cols.h"

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
    }

    ComponentSize ScreenComponent::GetActualComponentSize()
    {
        return size;
    }

    void ScreenComponent::MouseClicked(MouseClickEventInfo info)
    {
        selectedComponent = tempSelectedComponent;
        if (selectedComponent != NULL)
            selectedComponent->MouseClicked(info);
    }

    void ScreenComponent::KeyHit(KeyHitEventInfo info)
    {

    }

    void ScreenComponent::Destroy(bool destroyChildren)
    {
        renderer->Free();
        if (destroyChildren)
            for (int i = 0; i < children->getCount(); i++)
                children->elementAt(i)->Destroy(true);
        children->free();
    }
    
}