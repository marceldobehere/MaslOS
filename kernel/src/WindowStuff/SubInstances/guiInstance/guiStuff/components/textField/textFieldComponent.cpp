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

        componentType = ComponentType::TEXTFIELD;

        this->textCol = textCol;
        this->bgCol= bgCol;

        this->position = position;
        this->parent = parent;
        this->size = size;

        actualButtonStuff = new BoxComponent(parent, size, Colors.tblack);

        this->rectComp = new RectangleComponent(bgCol, size, actualButtonStuff);
        actualButtonStuff->children->add(rectComp);

        char* txt = (char*)malloc(1);
        txt[0] = 0;
        this->textComp = new TextComponent(actualButtonStuff, Colors.tblack, textCol, txt, Position());
        textComp->center = false;
        textComp->useFixedSize = true;
        textComp->size = size;
        actualButtonStuff->children->add(textComp);

        mouseClickedCallBack = NULL;
        keyHitCallBack = NULL;
    }

    void TextFieldComponent::MouseClicked(MouseClickEventInfo info)
    {
        if (mouseClickedCallBack != NULL)
            mouseClickedCallBack(this, info);
    }

    void TextFieldComponent::KeyHit(KeyHitEventInfo info)
    {
        KeyHandler(info);
        if (keyHitCallBack != NULL)
            keyHitCallBack(this, info);
    }

    void TextFieldComponent::Render(Field field)
    {
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

        actualButtonStuff->position = position;
        actualButtonStuff->size = tSize;
        rectComp->size = size;

        
            textComp->fgColor = textCol;
            rectComp->fillColor = bgCol;

        actualButtonStuff->Render(field);
    }

    void TextFieldComponent::Destroy(bool destroyChildren)
    {
        actualButtonStuff->Destroy(destroyChildren);
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
            char* bleh = (char*)malloc(len);
            for (int i = 0; i < len - 1; i++)
                bleh[i] = (*txt)[i];
            bleh[len - 1] = 0;

            AddToStack();
            tryFree((void*)*txt);
            RemoveFromStack();
            *txt = bleh; 
        }
        else
        {
            
            char* bleh = (char*)malloc(len + 2);
            for (int i = 0; i < len; i++)
                bleh[i] = (*txt)[i];
            bleh[len] = info.Chr;
            bleh[len + 1] = 0;

            AddToStack();
            tryFree((void*)*txt);
            RemoveFromStack();
            *txt = bleh; 
        }


        RemoveFromStack();
    }
}