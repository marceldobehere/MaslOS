#include "guiInstance.h"


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