#include "textFieldComponent.h"
#include "../../../../../../Rendering/Cols.h"
//#include "../../../../../../OSDATA/osdata.h"
#include "../../../../../../userinput/mouse.h"
#include "../screenComponent/screenComponent.h"
#include "../../../../../../memory/heap.h"
#include "../../../../../../OSDATA/MStack/MStackM.h"



namespace GuiComponentStuff
{
    TextFieldComponent::TextFieldComponent(uint32_t textCol, uint32_t bgCol, ComponentSize size, Position position, BaseComponent* parent)
    {
        AdvancedKeyHitCallBack = NULL;
        keyHitCallBack = NULL;
        mouseClickedCallBack = NULL;
        componentType = ComponentType::TEXTFIELD;
        updateFields = new List<Field>(5);

        this->textCol = textCol;
        this->bgCol= bgCol;

        this->position = position;
        this->parent = parent;
        this->size = size;

        actualTextFieldStuff = new BoxComponent(parent, size, Colors.tblack);

        this->rectComp = new RectangleComponent(bgCol, size, actualTextFieldStuff);
        actualTextFieldStuff->children->add(rectComp);


        this->textComp = new TextComponent(actualTextFieldStuff, Colors.tblack, textCol, "", Position());
        //_Free(txt);
        textComp->center = false;
        textComp->useFixedSize = true;
        textComp->size = size;
        actualTextFieldStuff->children->add(textComp);

        mouseClickedCallBack = NULL;
        keyHitCallBack = NULL;

        CheckUpdates();
        //Render(Field(Position(), GetActualComponentSize()));
    }

    void TextFieldComponent::MouseClicked(MouseClickEventInfo info)
    {
        if (mouseClickedCallBack != NULL)
            mouseClickedCallBack(this, info);
    }

    void TextFieldComponent::KeyHit(KeyHitEventInfo info)
    {
        bool yes = true;
        if (AdvancedKeyHitCallBack != NULL)
            yes = AdvancedKeyHitCallBack(AdvancedKeyHitCallBackHelp, this, info);

        if (yes)
            KeyHandler(info);
        if (keyHitCallBack != NULL)
            keyHitCallBack(this, info);
    }

    void TextFieldComponent::CheckUpdates()
    {
        AddToStack();

        Window* wind = (Window*)GetWindow();
        ::Position mPos = wind->GetMousePosRelativeToWindow();
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

        actualTextFieldStuff->position = position;
        actualTextFieldStuff->size = tSize;
        textComp->size = tSize;
        rectComp->size = tSize;

        
        textComp->fgColor = textCol;
        rectComp->fillColor = bgCol;

        actualTextFieldStuff->CheckUpdates();

        RemoveFromStack();
    }

    void TextFieldComponent::Render(Field field)
    {
        AddToStack();
        actualTextFieldStuff->Render(field);
        
        RemoveFromStack();
    }

    void TextFieldComponent::Destroy(bool destroyChildren, void (*callBackFunc)(BaseComponent* comp))
    {
        AddToStack();
        if (callBackFunc != NULL)
            callBackFunc(this);
        actualTextFieldStuff->Destroy(destroyChildren, callBackFunc);
        _Free(actualTextFieldStuff);
        updateFields->free();
        _Free(updateFields);
        RemoveFromStack();
    }

    ComponentSize TextFieldComponent::GetActualComponentSize()
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



    void TextFieldComponent::KeyHandler(GuiComponentStuff::KeyHitEventInfo info)
    {
        AddToStack();

        const char** txt = &textComp->text;

        int len = StrLen(*txt);
        if (info.Scancode == 0x0E)
        {
            if (len < 1)
            {
                RemoveFromStack();
                return;
            }
            char* bleh = (char*)_Malloc(len);
            for (int i = 0; i < len - 1; i++)
                bleh[i] = (*txt)[i];
            bleh[len - 1] = 0;

            AddToStack();
            _TryFree((void*)*txt);
            RemoveFromStack();
            *txt = bleh; 
        }
        else
        {
            
            char* bleh = (char*)_Malloc(len + 2);
            for (int i = 0; i < len; i++)
                bleh[i] = (*txt)[i];
            bleh[len] = info.Chr;
            bleh[len + 1] = 0;

            AddToStack();
            _TryFree((void*)*txt);
            RemoveFromStack();
            *txt = bleh; 
        }


        RemoveFromStack();
    }


    bool TextFieldComponent::SetAttribute(int32_t type, uint64_t val)
    {
        if (type == 10)
        {
            textCol = *((uint32_t*)&val);
            return true;
        }
        if (type == 11)
        {
            bgCol = *((uint32_t*)&val);
            return true;
        }

        if (type == 21)
        {
            mouseHover= *((bool*)&val);
            return true;
        }
        if (type == 22)
        {
            mouseClick= *((bool*)&val);
            return true;
        }

        return false;
    }

    uint64_t TextFieldComponent::GetAttribute(int32_t type)
    {
        uint8_t temp[8];
        for (int i = 0; i < 8; i++)
            temp[i] = 0;

        if (type == 10)
            *((uint32_t*)temp) = textCol;
        else if (type == 11)
            *((uint32_t*)temp) = bgCol;


        else if (type == 20)
            *((bool*)temp) = mouseHover;
        else if (type == 21)
            *((bool*)temp) = mouseClick;

        else if (type == 30)
            *((int64_t*)temp) = actualTextFieldStuff->id;
        else if (type == 31)
            *((int64_t*)temp) = rectComp->id;
        else if (type == 32)
            *((int64_t*)temp) = textComp->id;

        return *((uint64_t*)temp);
    }

    int TextFieldComponent::GetAttributeSize(int32_t type)
    {
        if (type == 10 || type == 11)
            return 4;
        if (type == 20 || type == 21)
            return 1;
        if (type == 30 || type == 31 || type == 32)
            return 8;


        return 0;
    }
}