#include "boxComponent.h"


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

    void BoxComponent::MouseClicked(Position mousePos)
    {

    }
    
    void BoxComponent::Render(Field field)
    {
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
    }

    void BoxComponent::Destroy(bool destroyChildren)
    {
        renderer->Free();
        if (destroyChildren)
            for (int i = 0; i < children->getCount(); i++)
                children->elementAt(i)->Destroy(true);
        children->free();
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

}