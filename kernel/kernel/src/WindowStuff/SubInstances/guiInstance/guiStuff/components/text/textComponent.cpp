#include "textComponent.h"
#include "../../../../../../cStdLib/cstrTools.h"
#include "../../../../../../memory/heap.h"
#include "../../../../../../osData/MStack/MStackM.h"

namespace GuiComponentStuff
{
    TextComponent::TextComponent(BaseComponent* parent, uint32_t bgColor, uint32_t fgColor, const char* text, Position position)
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

        this->bgColor = bgColor;
        this->fgColor = fgColor;
        this->text = StrCopy(text);
        oldText = StrCopy(text);
        updateFields = new List<Field>(5);

        this->size = ComponentSize(10, 10);
        componentType = ComponentType::TEXT;
        this->parent = parent;

        ComponentSize temp = GetActualComponentSize();
        renderer = new ComponentRenderer(temp);
        renderer->bgCol = bgColor;
        renderer->Fill(bgColor);
        this->position = position;
        oldPosition = position;
        oldSize = temp;
        renderer->PrintString(text, Position(0, 0), fgColor, bgColor, false, center);
        bgColorOld = bgColor;
        fgColorOld = fgColor;
        center = false;
        oldCenter = false;


        CheckUpdates();
        //Render(Field(Position(), GetActualComponentSize()));
    }

    void TextComponent::MouseClicked(MouseClickEventInfo info)
    {

    }

    void TextComponent::KeyHit(KeyHitEventInfo info)
    {

    }

    bool TextComponent::TextUpdate()
    {
        if (StrEquals(text, oldText))
            return false;
        _Free((void*)oldText);
        oldText = StrCopy(text);
        return true;
    }

    void TextComponent::CheckUpdates()
    {
        AddToStack();
        bool update = false;
        if (!useFixedSize)
        {

            ComponentSize temp = GetActualComponentSize();
            bool newText = TextUpdate();



            if (oldSize != temp || newText ||
                fgColor != fgColorOld || bgColor != bgColorOld ||
                oldCenter != center)
            {
                renderer->Resize(temp);
                renderer->Fill(bgColor);

                oldSize = temp;
                renderer->PrintString(text, Position(0, 0), fgColor, bgColor, false, center);
                fgColorOld = fgColor;
                bgColorOld = bgColor;
                oldCenter = center;
                update = true;
            }
        }
        else
        {

            bool newText = TextUpdate();
            if (oldSize != size || newText ||
                fgColor != fgColorOld || bgColor != bgColorOld ||
                oldCenter != center)
            {
                renderer->Resize(size);
                renderer->Fill(bgColor);
                oldSize = size;
                renderer->PrintString(text, Position(0, 0), fgColor, bgColor, false, center);
                fgColorOld = fgColor;
                bgColorOld = bgColor;
                oldCenter = center;
                update = true;
            }
        }

        if (update)
            parent->updateFields->add(Field(position, GetActualComponentSize()));

        RemoveFromStack();
    }

    void TextComponent::Render(Field field)
    {
        AddToStack();
        if (!hidden)
            renderer->Render(position, field, parent->renderer->componentFrameBuffer);
        RemoveFromStack();
    }

    void TextComponent::Destroy(bool destroyChildren, void (*callBackFunc)(BaseComponent* comp))
    {
        AddToStack();
        if (callBackFunc != NULL)
            callBackFunc(this);
        renderer->Free();
        _Free(oldText);
        _Free(text);
        updateFields->free();
        _Free(updateFields);
        RemoveFromStack();
    }

    ComponentSize TextComponent::GetActualComponentSize()
    {
        if (useFixedSize)
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

        int sX = 0;
        int sY = 0;
        int maxX = 0;

        for (int i = 0; text[i] != 0; i++)
        {
            char chr = text[i];
            sX += 8;
            if (chr == '\n')
            {
                sY += 16;
                if (sX > maxX)
                    maxX = sX;
                sX = 0;
            }
        }
        if (sX > maxX)
            maxX = sX;

        return ComponentSize(maxX, sY + 16);
    }


    bool TextComponent::SetAttribute(int32_t type, uint64_t val)
    {
        if (type == 10)
        {
            fgColor = *((uint32_t*)&val);
            return true;
        }
        if (type == 11)
        {
            bgColor = *((uint32_t*)&val);
            return true;
        }

        if (type == 20)
        {
            const char* nT = StrCopy(*((const char**)&val));
            _TryFree((void*)text);
            text = nT;
            return true;
        }

        if (type == 30)
        {
            center = *((bool*)&val);
            return true;
        }
        if (type == 31)
        {
            useFixedSize = *((bool*)&val);
            return true;
        }

        return false;
    }

    uint64_t TextComponent::GetAttribute(int32_t type)
    {
        uint8_t temp[8];
        for (int i = 0; i < 8; i++)
            temp[i] = 0;

        if (type == 10)
            *((uint32_t*)temp) = fgColor;
        else if (type == 11)
            *((uint32_t*)temp) = bgColor;

        else if (type == 20)
            *((uint64_t*)temp) = (uint64_t)text;

        else if (type == 30)
            *((bool*)temp) = center;
        else if (type == 31)
            *((bool*)temp) = useFixedSize;

        return *((uint64_t*)temp);
    }

    int TextComponent::GetAttributeSize(int32_t type)
    {
        if (type == 10)
            return 4;
        if (type == 11)
            return 4;

        if (type == 20)
            return 8;

        if (type == 30)
            return 1;
        if (type == 31)
            return 1;

        return 0;
    }

}