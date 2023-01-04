#include "buttonComponent.h"
#include "../../../../../../Rendering/Cols.h"
//#include "../../../../../../OSDATA/osdata.h"
#include "../../../../../../userinput/mouse.h"
#include "../screenComponent/screenComponent.h"



namespace GuiComponentStuff
{
    ButtonComponent::ButtonComponent(const char* text, uint32_t textColDef, uint32_t textColHover, uint32_t textColClick, uint32_t bgColDef, uint32_t bgColHover, uint32_t bgColClick, ComponentSize size, Position position, BaseComponent* parent)
    {

        componentType = ComponentType::BUTTON;

        this->textColDef = textColDef;
        this->textColHover = textColHover;
        this->textColClick = textColClick;

        this->bgColDef = bgColDef;
        this->bgColHover = bgColHover;
        this->bgColClick = bgColClick;

        this->position = position;
        this->parent = parent;
        this->size = size;

        actualButtonStuff = new BoxComponent(parent, size, Colors.tblack);

        this->rectComp = new RectangleComponent(bgColDef, size, actualButtonStuff);
        actualButtonStuff->children->add(rectComp);

        this->textComp = new TextComponent(actualButtonStuff, Colors.tblack, textColDef, text, Position());
        textComp->center = true;
        actualButtonStuff->children->add(textComp);

        mouseClickedCallBack = NULL;
    }

    void ButtonComponent::MouseClicked(MouseClickEventInfo info)
    {
        if (mouseClickedCallBack != NULL)
            mouseClickedCallBack(this, info);
    }

    void ButtonComponent::KeyHit(KeyHitEventInfo info)
    {

    }

    void ButtonComponent::Render(Field field)
    {
        Window* wind = (Window*)GetWindow();
        ::Position mPos = wind->GetMousePosRelativeToWindow();
        //position = Position(mPos.x, mPos.y);
        Position abs = GetAbsoluteComponentPosition();
        ScreenComponent* scr = (ScreenComponent*)GetScreen();

        ComponentSize tSize = GetActualComponentSize();

        mouseHover =
        mPos.x >= abs.x &&
        mPos.y >= abs.y &&
        mPos.x < abs.x + tSize.FixedX &&
        mPos.y < abs.y + tSize.FixedY;

        mouseHover &= !wind->hidden && activeWindow == wind;
        mouseClick = mouseHover && MouseClickState[0];

        if (mouseHover)
            scr->tempSelectedComponent = this;

        actualButtonStuff->position = position;
        actualButtonStuff->size = tSize;
        rectComp->size = size;

        ComponentSize textSize = textComp->GetActualComponentSize();

        if (textSize.FixedX < tSize.FixedX)
            textComp->position.x = (tSize.FixedX - textSize.FixedX) / 2;
        else
            textComp->position.x = 0;

        if (textSize.FixedY < tSize.FixedY)
            textComp->position.y = (tSize.FixedY - textSize.FixedY) / 2;
        else
            textComp->position.y = 0;

        

        if (mouseHover && !stickToDefaultColor)
        {
            if (mouseClick)
            {
                textComp->fgColor = textColClick;
                rectComp->fillColor = bgColClick;
            }
            else
            {
                textComp->fgColor = textColHover;
                rectComp->fillColor = bgColHover;
            }
        }
        else
        {
            textComp->fgColor = textColDef;
            rectComp->fillColor = bgColDef;
        }

        actualButtonStuff->Render(field);
    }

    void ButtonComponent::Destroy(bool destroyChildren)
    {
        actualButtonStuff->Destroy(destroyChildren);
    }

    ComponentSize ButtonComponent::GetActualComponentSize()
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