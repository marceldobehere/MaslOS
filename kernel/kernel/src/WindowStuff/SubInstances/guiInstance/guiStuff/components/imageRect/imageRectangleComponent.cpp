#include "imageRectangleComponent.h"
#include "../../../../../../OSDATA/MStack/MStackM.h"
#include "../../../../../../fsStuff/fsStuff.h"

namespace GuiComponentStuff
{
    ImageRectangleComponent::ImageRectangleComponent(const char* path, ComponentSize size, BaseComponent* parent)
    {
        this->imagePath = StrCopy(path);
        this->oldPath = StrCopy(path);
        //this->fillColor = fillColor;
        this->size = size;
        componentType = IMAGE_RECT;
        this->parent = parent;
        ComponentSize temp = GetActualComponentSize();
        renderer = new ComponentRenderer(temp);
        renderer->bgCol = Colors.black;
        renderer->Fill(Colors.black);
        oldPosition = position;
        oldSize = temp;
        //oldFillColor = fillColor;
        updateFields = new List<Field>(5);

        image = NULL;
        GetImageFromPath(imagePath);
        RenderImg();

        CheckUpdates();
        //Render(Field(Position(), GetActualComponentSize()));
    }

    void ImageRectangleComponent::RenderImg()
    {
        AddToStack();
        renderer->Fill(Colors.black);
        if (image == NULL)
        {
            RemoveFromStack();
            return;
        }

        // RemoveFromStack();
        // return;
        //renderer->Fill(Colors.bgreen);

        // GlobalRenderer->Clear(Colors.black);
        // GlobalRenderer->Println("SIZE 1:");
        // GlobalRenderer->Println(" W: {}:", to_string(size.FixedX), Colors.white);
        // GlobalRenderer->Println(" H: {}:", to_string(size.FixedY), Colors.white);
        // GlobalRenderer->Println("SIZE 2:");
        // GlobalRenderer->Println(" W: {}:", to_string(image->width), Colors.white);
        // GlobalRenderer->Println(" H: {}:", to_string(image->height), Colors.white);
        // while (true);


        // if (tSize.FixedX != image->width || 
        //     tSize.FixedY != image->height)
        //     return;
        //renderer->Fill(Colors.yellow);
        

        // for (int y = 0; y < image->height; y++)
        // {
        //     int w = renderer->componentFrameBuffer->Width;
        //     for (int x = 0; x < image->width; x++)
        //     {
        //         uint32_t col = ((uint32_t*)image->imageBuffer)[y * image->width + x];
        //         renderer->componentFrameBuffer->pixels[y * w + x] = col;

        //     }
        // }

        // draw scaled

        ComponentSize tSize = GetActualComponentSize();

        int w = tSize.FixedX;
        int h = tSize.FixedY;

        int w2 = image->width;
        int h2 = image->height;

        if (w <= 0 || h <= 0 || w2 <= 0 || h2 <= 0)
        {
            RemoveFromStack();
            return;
        }



        for (int y = 0; y < h; y++)
        {
            for (int x = 0; x < w; x++)
            {
                int aX = (x * w2) / w;
                int aY = (y * h2) / h;
                uint32_t col = ((uint32_t*)image->imageBuffer)[aY * w2 + aX];

                renderer->componentFrameBuffer->pixels[y * w + x] = col;
            }
        }

        RemoveFromStack();
    }

    void ImageRectangleComponent::GetImageFromPath(const char* path)
    {
        AddToStack();
        if (image != NULL)
        {
            _Free(image->imageBuffer);
            _Free(image);
        }
        image = NULL;

        char* buf;
        int size = 0;
        if (!FS_STUFF::LoadFileFromFullPath(path, &buf, &size))
        {
            RemoveFromStack();
            return;
        }

        kernelFiles::DefaultFile tempFile;
        tempFile.fileData = buf;
        tempFile.size = size;
        tempFile.filename = (char*)"A";
        tempFile.filenameSize = 1;
        kernelFiles::ImageFile* img = kernelFiles::ConvertFileToImage(&tempFile);
        if (img == NULL)
        {
            _Free(buf);
            RemoveFromStack();
            return;
        }

        _Free(buf);
        image = img;
        RemoveFromStack();
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
            oldSize = temp;
            //renderer->Fill(Colors.black);
            RenderImg();
            update = true;
        }
        if (!StrEquals(imagePath, oldPath))
        {
            _Free((void*)oldPath);
            oldPath = StrCopy(imagePath);
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
        if (image != NULL)
        {
            _Free(image->imageBuffer);
            _Free(image);
        }
        image = NULL;
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
        if (type == 10)
        {
            _Free(imagePath);
            imagePath = StrCopy((const char*)val);
            return true;
        }

        return false;
    }

    uint64_t ImageRectangleComponent::GetAttribute(int32_t type)
    {
        uint8_t temp[8];
        for (int i = 0; i < 8; i++)
            temp[i] = 0;

        if (type == 10)
            *((uint64_t*)temp) = (uint64_t)imagePath;
        if (type == 20 && image != NULL)
            *((uint32_t*)temp) = image->width;
        if (type == 21 && image != NULL)
            *((uint32_t*)temp) = image->height;

        return *((uint64_t*)temp);
    }

    int ImageRectangleComponent::GetAttributeSize(int32_t type)
    {
        if (type == 10)
            return 8;
        if (type == 20)
            return 4;
        if (type == 21)
            return 4;

        return 0;
    }

}