#pragma once
#include "../base/baseComponent.h"
#include "../../../../../Window/window.h"
#include "../../../../../../customClasses/list.h"
#include "../../../../../../kernelStuff/kernelFiles/ImageStruct.h"

namespace GuiComponentStuff
{
    enum class GuiInstanceImageRectComponentAttributeType : int32_t
    {
        FILL_COLOR = 10
    };

    class ImageRectangleComponent : public BaseComponent
    {
        public:
        const char* imagePath = "";
        const char* oldPath = "";
        kernelFiles::ImageFile* image = NULL;
        
        void RenderImg();
        void GetImageFromPath(const char* path);

        ImageRectangleComponent(const char* path, ComponentSize size, BaseComponent* parent);
        void MouseClicked(MouseClickEventInfo info);
        void KeyHit(KeyHitEventInfo info);
        void Render(Field field);
        void CheckUpdates();
        void Destroy(bool destroyChildren, void (*callBackFunc)(BaseComponent* comp));
        ComponentSize GetActualComponentSize();

        bool SetAttribute(int32_t type, uint64_t val);
        uint64_t GetAttribute(int32_t type);
        int GetAttributeSize(int32_t type);
    };
}