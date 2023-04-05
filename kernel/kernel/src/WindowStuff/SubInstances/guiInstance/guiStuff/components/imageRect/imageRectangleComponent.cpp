#include "imageRectangleComponent.h"
#include "../../../../../../OSDATA/MStack/MStackM.h"

namespace GuiComponentStuff
{
    ImageRectangleComponent::ImageRectangleComponent(const char* path, ComponentSize size, BaseComponent* parent)
    {
        this->imagePath = StrCopy(path);
        this->oldPath = StrCopy(path);
        //this->fillColor = fillColor;
        this->size = size;
        componentType = RECT;
        this->parent = parent;
        ComponentSize temp = GetActualComponentSize();
        renderer = new ComponentRenderer(temp);
        renderer->bgCol = Colors.black;
        renderer->Fill(Colors.black);
        oldPosition = position;
        oldSize = temp;
        //oldFillColor = fillColor;
        updateFields = new List<Field>(5);

        GetImageFromPath(imagePath);
        RenderImg();

        CheckUpdates();
        //Render(Field(Position(), GetActualComponentSize()));
    }

    void ImageRectangleComponent::RenderImg()
    {
        renderer->Fill(Colors.black);

    }
    void ImageRectangleComponent::GetImageFromPath(const char* path)
    {

    }

    void ImageRectangleComponent::MouseClicked(MouseClickEventInfo info)
    {

    }

    void ImageRectangleComponent::KeyHit(KeyHitEventInfo info)
    {

    }

    void ImageRectangleComponent::CheckUpdates()
    {
        AddToStack();
        bool update = false;
        ComponentSize temp = GetActualComponentSize();
        if (oldSize != temp)
        {
            renderer->Resize(temp);
            renderer->Fill(Colors.black);
            RenderImg();
            oldSize = temp;
            update = true;
        }
        if (!StrEquals(imagePath, oldPath))
        {
            _Free((void*)oldPath);
            oldPath = StrCopy(imagePath);
            if (image != NULL)
                _Free(image);
            GetImageFromPath(imagePath);
            RenderImg();
            
            update = true;
        }
        // {
        //     renderer->Fill(fillColor);
        //     oldFillColor = fillColor;
        //     update = true;
        // }

        if (update)
            parent->updateFields->add(Field(position, temp));

        RemoveFromStack();
    }

    void ImageRectangleComponent::Render(Field field)
    {
        AddToStack();
        if (!hidden)
            renderer->Render(position, field, parent->renderer->componentFrameBuffer);
        RemoveFromStack();
    }

    void ImageRectangleComponent::Destroy(bool destroyChildren, void (*callBackFunc)(BaseComponent* comp))
    {
        AddToStack();
        if (callBackFunc != NULL)
            callBackFunc(this);
        renderer->Free();
        updateFields->free();
        _Free(updateFields);
        _Free((void*)imagePath);
        _Free((void*)oldPath);
        RemoveFromStack();
    }

    ComponentSize ImageRectangleComponent::GetActualComponentSize()
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

    bool ImageRectangleComponent::SetAttribute(int32_t type, uint64_t val)
    {
        // if (type == 10)
        // {
        //     fillColor = *((uint32_t*)&val);
        //     return true;
        // }

        return false;
    }

    uint64_t ImageRectangleComponent::GetAttribute(int32_t type)
    {
        uint8_t temp[8];
        for (int i = 0; i < 8; i++)
            temp[i] = 0;

        // if (type == 10)
        //     *((uint32_t*)temp) = fillColor;

        return *((uint64_t*)temp);
    }

    int ImageRectangleComponent::GetAttributeSize(int32_t type)
    {
        if (type == 10)
            return 4;

        return 0;
    }

}