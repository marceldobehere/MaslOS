#include "boxComponent.h"


namespace GuiComponentStuff
{
    BoxComponent::BoxComponent(BaseComponent* parent)
    {
        this->parent = parent;
        this->size = ComponentSize(true);
        oldSize = size;
        componentType = ComponentType::BOX;
        children = new List<BaseComponent*>(5);

        ComponentSize temp = GetActualComponentSize(this);

        renderer = new ComponentRenderer(temp);
        renderer->bgCol = backgroundColor;
        renderer->Fill(backgroundColor);
    }

    BoxComponent::BoxComponent(BaseComponent* parent, ComponentSize size)
    {
        this->parent = parent;
        this->size = size;
        oldSize = size;
        componentType = ComponentType::BOX;
        children = new List<BaseComponent*>(5);

        ComponentSize temp = GetActualComponentSize(this);

        renderer = new ComponentRenderer(temp);
        renderer->bgCol = backgroundColor;
        renderer->Fill(backgroundColor);
    }

    void BoxComponent::MouseClicked(Position mousePos)
    {

    }
    
    void BoxComponent::Render(Field field)
    {
        ComponentSize temp = GetActualComponentSize(this);
        if (oldSize != temp)
        {
            renderer->Resize(temp);
            renderer->Fill(backgroundColor);
            oldSize = temp;
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

    ComponentSize BoxComponent::GetActualComponentSize(BaseComponent* caller)
    {
        if ((size.undefined || !size.IsXFixed || !size.IsYFixed))
        {
            if (children->getIndexOf(caller) != -1)
            {
                return parent->GetActualComponentSize(this);
            }
            else
            {
                int sY = 0;
                int sX = 0;
                int maxY = 0;
                int maxX = 0;
                for (int i = 0; i < children->getCount(); i++)
                {
                    BaseComponent* child = children->elementAt(i);
                    ComponentSize temp = child->GetActualComponentSize(this);
                    
                    sX += temp.FixedX;
                    if (sX > maxX)
                        maxX = sX;
                    
                    if (sY + temp.FixedY > maxY)
                        maxY = sY + temp.FixedY;
                    
                    if (child->newLine)
                    {
                        sX = 0;
                        sY += maxY;
                    }
                }

                return ComponentSize(maxX, maxY);
            }
        }
        else
            return size;
    }

}