#include "screenComponent.h"
#include "../../../../../../Rendering/Cols.h"

namespace GuiComponentStuff
{
    ScreenComponent::ScreenComponent(Window* window)
    {
        this->window = window;
        wantedSize = ComponentSize((int)window->framebuffer->Width, (int)window->framebuffer->Height);
        actualSize = wantedSize;
        componentType = SCREEN;
        parent = NULL;
        children = new List<BaseComponent*>(5);
        renderer = new ComponentRenderer(actualSize);
        renderer->bgCol = Colors.bgray;
        renderer->Fill(Colors.bgray);
    }

    void ScreenComponent::Render(Field field)
    {
        ComponentSize tSize = ComponentSize((int)window->framebuffer->Width, (int)window->framebuffer->Height);

        if (tSize.FixedX != actualSize.FixedX || tSize.FixedY != actualSize.FixedY)
        {
            renderer->Resize(tSize);
            actualSize = tSize;
            wantedSize = tSize;
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
        return actualSize;
    }

    void ScreenComponent::MouseClicked(Position mousePos)
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