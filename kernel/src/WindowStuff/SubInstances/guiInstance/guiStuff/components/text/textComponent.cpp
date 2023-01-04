#include "textComponent.h"
#include "../../../../../../cmdParsing/cstrTools.h"
#include "../../../../../../memory/heap.h"


namespace GuiComponentStuff
{
    TextComponent::TextComponent(BaseComponent* parent, uint32_t bgColor, uint32_t fgColor, const char* text, Position position)
    {
        this->bgColor = bgColor;
        this->fgColor = fgColor;
        this->text = text;
        oldText = StrCopy(text);

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
    }

    void TextComponent::MouseClicked(Position mousePos)
    {

    }

    bool TextComponent::TextUpdate()
    {
        if (StrEquals(text, oldText))
            return false;
        free((void*)oldText);
        oldText = StrCopy(text);
        return true;
    }

    void TextComponent::Render(Field field)
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
        }


        renderer->Render(position, field, parent->renderer->componentFrameBuffer);
    }

    void TextComponent::Destroy(bool destroyChildren)
    {
        renderer->Free();
    }

    ComponentSize TextComponent::GetActualComponentSize()
    {
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

}