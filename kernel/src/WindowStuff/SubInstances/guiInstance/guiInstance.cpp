#include "guiInstance.h"
#include "guiStuff/components/box/boxComponent.h"
#include "guiStuff/components/button/buttonComponent.h"
#include "guiStuff/components/screenComponent/screenComponent.h"
#include "guiStuff/components/textField/textFieldComponent.h"

GuiInstance::GuiInstance(Window* window)
{
    this->window = window;
    instanceType = InstanceType::GUI;
}

void GuiInstance::Free() 
{
    screen->Destroy(true);
}

void GuiInstance::Init()
{
    window->renderer->Clear(Colors.bgreen);
    screen = new GuiComponentStuff::ScreenComponent(window);
    window->renderer->Clear(Colors.orange);
}

void GuiInstance::Render()
{
    //window->renderer->Clear(Colors.orange);
    screen->Render(GuiComponentStuff::Field(GuiComponentStuff::Position(), GuiComponentStuff::Position(window->size.width, window->size.height)));
    long t = window->size.height * (long)window->size.width;

    for (long i = 0; i < t; i++)
        ((uint32_t*)window->framebuffer->BaseAddress)[i] = screen->renderer->componentFrameBuffer->pixels[i];
}

GuiComponentStuff::BaseComponent* GuiInstance::GetComponentFromId(uint64_t id)
{
    if (screen == NULL)
        return NULL;

    return screen->GetComponentFromId(id);
}

GuiComponentStuff::BaseComponent* GuiInstance::GetChildFromComponentWithId(uint64_t id, int index)
{
    GuiComponentStuff::BaseComponent* base = GetComponentFromId(id);
    if (base == NULL)
        return NULL;
    if (index < 0)
        return NULL;

    if (base->componentType == GuiComponentStuff::ComponentType::BOX)
    {
        GuiComponentStuff::BoxComponent* box = (GuiComponentStuff::BoxComponent*)base;
        if (index >= box->children->getCount())
            return NULL;
        return box->children->elementAt(index);
    }

    if (base->componentType == GuiComponentStuff::ComponentType::SCREEN)
    {
        GuiComponentStuff::ScreenComponent* scr = (GuiComponentStuff::ScreenComponent*)base;
        if (index >= scr->children->getCount())
            return NULL;
        return scr->children->elementAt(index);
    }

    if (base->componentType == GuiComponentStuff::ComponentType::BUTTON)
    {
        GuiComponentStuff::ButtonComponent* btn = (GuiComponentStuff::ButtonComponent*)base;
        if (index >= 1)
            return NULL;
        return btn->actualButtonStuff;
    }

    if (base->componentType == GuiComponentStuff::ComponentType::TEXTFIELD)
    {
        GuiComponentStuff::TextFieldComponent* txt = (GuiComponentStuff::TextFieldComponent*)base;
        if (index >= 1)
            return NULL;
        return txt->actualButtonStuff;
    }

    return NULL;
}

bool GuiInstance::DeleteComponentWithId(int64_t id, bool destroyChildren)
{
    GuiComponentStuff::BaseComponent* base = GetComponentFromId(id);
    if (base == NULL)
        return false;

    base->Destroy(destroyChildren);
    return true;
}


bool GuiInstance::SetActiveScreenFromId(int64_t id)
{
    GuiComponentStuff::BaseComponent* base = GetComponentFromId(id);
    if (base == NULL)
        return false;
    if (base->componentType !=GuiComponentStuff::ComponentType::SCREEN)
        return false;

    screen = (GuiComponentStuff::ScreenComponent*)base;
    return true;
}

bool GuiInstance::SetSpecificComponentAttribute(int64_t id, int32_t type, uint64_t val)
{
    GuiComponentStuff::BaseComponent* base = GetComponentFromId(id);
    if (base == NULL)
        return false;

    return base->SetAttribute(type, val);
}

uint64_t GuiInstance::GetSpecificComponentAttribute(int64_t id, int32_t type)
{
    GuiComponentStuff::BaseComponent* base = GetComponentFromId(id);
    if (base == NULL)
        return 0;

    return base->GetAttribute(type);
}






bool GuiInstance::SetBaseComponentAttribute(int64_t id, GuiInstanceBaseAttributeType type, uint64_t val)
{
    GuiComponentStuff::BaseComponent* base = GetComponentFromId(id);
    if (base == NULL)
        return false;

    if (type == GuiInstanceBaseAttributeType::POSITION_X)
    {
        base->position.x = *((int32_t*)&val);
        return true;
    }

    if (type == GuiInstanceBaseAttributeType::POSITION_Y)
    {
        base->position.y = *((int32_t*)&val);
        return true;
    }

    if (type == GuiInstanceBaseAttributeType::SIZE_FIXED_X)
    {
        base->size.FixedX = *((int32_t*)&val);
        return true;
    }

    if (type == GuiInstanceBaseAttributeType::SIZE_FIXED_Y)
    {
        base->size.FixedY = *((int32_t*)&val);
        return true;
    }

    if (type == GuiInstanceBaseAttributeType::ACTUAL_SIZE_X)
    {
        //base->GetActualComponentSize().FixedX = *((int32_t*)&val);
        return false;
    }

    if (type == GuiInstanceBaseAttributeType::ACTUAL_SIZE_Y)
    {
        //base->GetActualComponentSize().FixedY = *((int32_t*)&val);
        return false;
    }

    if (type == GuiInstanceBaseAttributeType::SIZE_SCALED_X)
    {
        base->size.ScaledX = *((double*)&val);
        return true;
    }

    if (type == GuiInstanceBaseAttributeType::SIZE_SCALED_Y)
    {
        base->size.ScaledY = *((double*)&val);
        return true;
    }

    if (type == GuiInstanceBaseAttributeType::SIZE_IS_FIXED_X)
    {
        base->size.IsXFixed = *((bool*)&val);
        return true;
    }

    if (type == GuiInstanceBaseAttributeType::SIZE_IS_FIXED_Y)
    {
        base->size.IsYFixed = *((bool*)&val);
        return true;
    }

    if (type == GuiInstanceBaseAttributeType::ID)
    {
        base->id = *((int64_t*)&val);
        return true;
    }

    if (type == GuiInstanceBaseAttributeType::PARENT_ID)
    {
        return false;
        // if (base->parent == NULL)
        //     return false;

        // base->parent->id = *((int64_t*)&val);
        // return true;
    }

    if (type == GuiInstanceBaseAttributeType::IS_HIDDEN)
    {
        base->hidden= *((bool*)&val);
        return true;
    }

    return false;
}

uint64_t GuiInstance::GetBaseComponentAttribute(int64_t id, GuiInstanceBaseAttributeType type)
{
    GuiComponentStuff::BaseComponent* base = GetComponentFromId(id);
    if (base == NULL)
        return 0;

    uint8_t temp[8];
    for (int i = 0; i < 8; i++)
        temp[i] = 0;

    if (type == GuiInstanceBaseAttributeType::POSITION_X)
        *((int32_t*)temp) = base->position.x;
    if (type == GuiInstanceBaseAttributeType::POSITION_Y)
        *((int32_t*)temp) = base->position.y;

    if (type == GuiInstanceBaseAttributeType::SIZE_FIXED_X)
        *((int32_t*)temp) = base->size.FixedX;
    if (type == GuiInstanceBaseAttributeType::SIZE_FIXED_Y)
        *((int32_t*)temp) = base->size.FixedY;
    if (type == GuiInstanceBaseAttributeType::ACTUAL_SIZE_X)
            *((int32_t*)temp) = base->GetActualComponentSize().FixedX;
    if (type == GuiInstanceBaseAttributeType::ACTUAL_SIZE_Y)
            *((int32_t*)temp) = base->GetActualComponentSize().FixedY;
    if (type == GuiInstanceBaseAttributeType::SIZE_SCALED_X)
        *((double*)temp) = base->size.ScaledX;
    if (type == GuiInstanceBaseAttributeType::SIZE_SCALED_Y)
        *((double*)temp) = base->size.ScaledY;
    if (type == GuiInstanceBaseAttributeType::SIZE_IS_FIXED_X)
        *((bool*)temp) = base->size.IsXFixed;
    if (type == GuiInstanceBaseAttributeType::SIZE_IS_FIXED_Y)
        *((bool*)temp) = base->size.IsYFixed;

    if (type == GuiInstanceBaseAttributeType::ID)
        *((int64_t*)temp) = base->id;
    if (type == GuiInstanceBaseAttributeType::PARENT_ID)
        if (base->parent != NULL)
                *((int64_t*)temp) =  base->parent->id;
    if (type == GuiInstanceBaseAttributeType::IS_HIDDEN)
        *((bool*)temp) = base->hidden;

    return *((uint64_t*)temp);
}



bool GuiInstance::CreateComponentWithId(int64_t id, GuiComponentStuff::ComponentType type)
{
    if (screen == NULL)
        return false;
    return CreateComponentWithIdAndParent(id, type, screen->id);
}

bool GuiInstance::CreateComponentWithIdAndParent(int64_t id, GuiComponentStuff::ComponentType type, int64_t parentId)
{
    if (GetComponentFromId(id) != NULL)
        return false;

    return true;
}