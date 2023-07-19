#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../../generalStuff.h"
#include "../../componentRenderer.h"

namespace GuiComponentStuff
{
    class BaseComponent;
}

#include "../../../../../../cStdLib/list/list_guiField.h"

namespace GuiComponentStuff
{
    enum ComponentType : uint8_t
    {
        NONE = 0,
        SCREEN = 1,
        RECT = 2,
        BOX = 3,
        TEXT = 4,
        BUTTON = 5,
        TEXTFIELD = 6,
        IMAGE_RECT = 7,
        CANVAS = 8,
    };


    class BaseComponent
    {
        protected:
        

        public:
        List<Field>* updateFields;
        BaseComponent* parent;
        ComponentSize size = ComponentSize();
        Position position;
        ComponentSize oldSize = ComponentSize();
        Position oldPosition;
        //const char* ID;
        ComponentType componentType;
        ComponentRenderer* renderer;
        bool hidden = false;
        int64_t id = 10;
        




        public:


        void (*RenderFunc)(void* bruh, Field) = NULL;
        void (*CheckUpdatesFunc)(void* bruh) = NULL;
        void (*MouseClickedFunc)(void* bruh, MouseClickEventInfo) = NULL;
        void (*KeyHitFunc)(void* bruh, KeyHitEventInfo) = NULL;
        void (*DestroyFunc)(void* bruh, bool, void (*)(BaseComponent* comp)) = NULL;
        ComponentSize (*GetActualComponentSizeFunc)(void* bruh) = NULL;
        bool (*SetAttributeFunc)(void* bruh, int32_t, uint64_t) = NULL;
        uint64_t (*GetAttributeFunc)(void* bruh, int32_t) = NULL;
        int (*GetAttributeSizeFunc)(void* bruh, int32_t) = NULL;

        BaseComponent(BaseComponent* parent);
        BaseComponent();
        ComponentSize GetActualComponentSize();
        Position GetAbsoluteComponentPosition();
        void* GetWindow();
        void* GetScreen();
        bool IsVisible();
        //BaseComponent* GetComponentFromId(uint64_t id);

        void MouseClicked(MouseClickEventInfo info);
        void KeyHit(KeyHitEventInfo info);

        void Render(Field field);
        void CheckUpdates();
        bool Destroy(bool destroyChildren, void (*callBackFunc)(BaseComponent* comp));

        bool SetAttribute(int32_t type, uint64_t val);
        uint64_t GetAttribute(int32_t type);
        int GetAttributeSize(int32_t type);
        
        Field GetFieldWithPos();

    };


}