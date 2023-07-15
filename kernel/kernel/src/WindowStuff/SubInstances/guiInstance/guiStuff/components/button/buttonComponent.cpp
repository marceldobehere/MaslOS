#include "buttonComponent.h"
#include "../../../../../../rendering/Cols.h"
//#include "../../../../../../osData/osData.h"
#include "../../../../../../userinput/mouse.h"
#include "../screenComponent/screenComponent.h"
#include "../../../../../../osData/MStack/MStackM.h"



namespace GuiComponentStuff
{
    ButtonComponent::ButtonComponent(const char* text, uint32_t textColDef, uint32_t textColHover, uint32_t textColClick, uint32_t bgColDef, uint32_t bgColHover, uint32_t bgColClick, ComponentSize size, Position position, BaseComponent* parent)
    {
        OnMouseClickedCallBack = NULL;
        OnMouseClickHelp = NULL;
        mouseClickedCallBack = NULL;
        keyHitCallBack = NULL;
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

        updateFields = new List<Field>(5);

        actualButtonStuff = new BoxComponent(parent, size, Colors.tblack);

        this->rectComp = new RectangleComponent(bgColDef, size, actualButtonStuff);
        actualButtonStuff->children->add(rectComp);
        rectComp->id = 123401;

        this->textComp = new TextComponent(actualButtonStuff, Colors.tblack, textColDef, text, Position());
        textComp->center = true;
        actualButtonStuff->children->add(textComp);
        textComp->id = 123402;

        mouseClickedCallBack = NULL;
        keyHitCallBack = NULL;

        CheckUpdates();
        //Render(Field(Position(), GetActualComponentSize()));
    }

    void ButtonComponent::MouseClicked(MouseClickEventInfo info)
    {
        if (mouseClickedCallBack != NULL)
            mouseClickedCallBack(this, info);
        if (OnMouseClickedCallBack != NULL)
            OnMouseClickedCallBack(OnMouseClickHelp, this, info);
    }

    void ButtonComponent::KeyHit(KeyHitEventInfo info)
    {
        if (keyHitCallBack != NULL)
            keyHitCallBack(this, info);
    }

    void ButtonComponent::CheckUpdates()
    {
        AddToStack();
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
        rectComp->size = tSize;

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

        actualButtonStuff->CheckUpdates();

        RemoveFromStack();
    }

    void ButtonComponent::Render(Field field)
    {
        AddToStack();

        actualButtonStuff->Render(field);
        RemoveFromStack();
    }

    void ButtonComponent::Destroy(bool destroyChildren, void (*callBackFunc)(BaseComponent* comp))
    {
        AddToStack();
        if (callBackFunc != NULL)
            callBackFunc(this);
        actualButtonStuff->Destroy(destroyChildren, callBackFunc);
        _Free(actualButtonStuff);
        updateFields->free();
        _Free(updateFields);
        RemoveFromStack();
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


    bool ButtonComponent::SetAttribute(int32_t type, uint64_t val)
    {
        if (type == 10)
        {
            textColDef = *((uint32_t*)&val);
            return true;
        }
        if (type == 11)
        {
            textColHover = *((uint32_t*)&val);
            return true;
        }
        if (type == 12)
        {
            textColClick = *((uint32_t*)&val);
            return true;
        }

        if (type == 20)
        {
            bgColDef = *((uint32_t*)&val);
            return true;
        }
        if (type == 21)
        {
            bgColHover = *((uint32_t*)&val);
            return true;
        }
        if (type == 22)
        {
            bgColClick = *((uint32_t*)&val);
            return true;
        }

        if (type == 30)
        {
            stickToDefaultColor= *((bool*)&val);
            return true;
        }
        if (type == 31)
        {
            mouseHover= *((bool*)&val);
            return true;
        }
        if (type == 32)
        {
            mouseClick= *((bool*)&val);
            return true;
        }

        return false;
    }

    uint64_t ButtonComponent::GetAttribute(int32_t type)
    {
        uint8_t temp[8];
        for (int i = 0; i < 8; i++)
            temp[i] = 0;

        if (type == 10)
            *((uint32_t*)temp) = textColDef;
        else if (type == 11)
            *((uint32_t*)temp) = textColHover;
        else if (type == 12)
            *((uint32_t*)temp) = textColClick;


        else if (type == 20)
            *((uint32_t*)temp) = bgColDef;
        else if (type == 21)
            *((uint32_t*)temp) = bgColHover;
        else if (type == 22)
            *((uint32_t*)temp) = bgColClick;

        else if (type == 30)
            *((bool*)temp) = stickToDefaultColor;
        else if (type == 31)
            *((bool*)temp) = mouseHover;
        else if (type == 32)
            *((bool*)temp) = mouseClick;

        else if (type == 40)
            *((int64_t*)temp) = actualButtonStuff->id;
        else if (type == 41)
            *((int64_t*)temp) = rectComp->id;
        else if (type == 42)
            *((int64_t*)temp) = textComp->id;

        else if (type == 50)
        {
            int state = 0;
            if (mouseHover)
            {
                if (mouseClick)
                    state = 2;
                else
                    state = 1;
            }
            
            *((int32_t*)temp) = state;
        }

        return *((uint64_t*)temp);
    }

    int ButtonComponent::GetAttributeSize(int32_t type)
    {
        if (type == 10 || type == 11 || type == 12)
            return 4;
        if (type == 20 || type == 21 || type == 22)
            return 4;
        if (type == 30 || type == 31 || type == 32)
            return 1;
        if (type == 40 || type == 41 || type == 42)
            return 8;
        if (type == 50)
            return 4;

        return 0;
    }

}