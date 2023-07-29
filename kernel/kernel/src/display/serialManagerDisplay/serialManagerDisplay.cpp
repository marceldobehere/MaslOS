#include "serialManagerDisplay.h"
#include "../../cStdLib/cStdLib.h"

SerialManagerDisplay::SerialManagerDisplay(SerialManager::Manager* manager, Framebuffer* framebuffer)
{
    this->framebuffer = new Framebuffer();//framebuffer;
    this->framebuffer->Width = 640;
    this->framebuffer->Height = 480;
    this->framebuffer->PixelsPerScanLine = this->framebuffer->Width;
    this->framebuffer->BaseAddress = _Malloc(this->framebuffer->PixelsPerScanLine * this->framebuffer->Height * 4);
    _memset(this->framebuffer->BaseAddress, 0, this->framebuffer->PixelsPerScanLine * this->framebuffer->Height * 4);

    this->manager = manager;
    type = DisplayType::SerialManagerType;
    pixelUpdates = new Queue<SerialPixelUpdate>(5000);
}

void SerialManagerDisplay::StartFrame()
{
    pixelUpdates->Clear();
}

void SerialManagerDisplay::UpdatePixel(int x, int y)
{
    if (x < 0 || y < 0 || x > framebuffer->Width || y > framebuffer->Height)
        return;
    
    pixelUpdates->Enqueue(SerialPixelUpdate(x, y));

    Framebuffer* buf = GlobalRenderer->framebuffer;
    if (x > buf->Width || y > buf->Height)
        return;

    uint32_t col = *(((uint32_t*)framebuffer->BaseAddress) + x + y * framebuffer->PixelsPerScanLine);
    *(((uint32_t*)buf->BaseAddress) + x + y * buf->PixelsPerScanLine) = col;
    
}

void SerialManagerDisplay::EndFrame()
{
    if (pixelUpdates->GetCount() == 0)
        return;
    
    int dataLen = pixelUpdates->GetCount() * 7;
    uint8_t* data = (uint8_t*)_Malloc(dataLen);
    int dataIndex = 0;
    while (pixelUpdates->GetCount() > 0)
    {
        SerialPixelUpdate update = pixelUpdates->Dequeue();
        uint32_t col = *(((uint32_t*)framebuffer->BaseAddress) + update.x + update.y * framebuffer->PixelsPerScanLine);

        data[dataIndex++] = (uint8_t)(update.x & 0xFF);
        data[dataIndex++] = (uint8_t)((update.x >> 8) & 0xFF);
        
        data[dataIndex++] = (uint8_t)(update.y & 0xFF);
        data[dataIndex++] = (uint8_t)((update.y >> 8) & 0xFF);

        // COL IS ARGB
        data[dataIndex++] = (uint8_t)((col >> 16) & 0xFF);
        data[dataIndex++] = (uint8_t)((col >> 8) & 0xFF);
        data[dataIndex++] = (uint8_t)(col & 0xFF);
    
    }

    SerialManager::GenericPacket* packet = new 
    SerialManager::GenericPacket(
        SerialManager::PacketType::DATA,
        SerialManager::ReservedHostPortsEnum::VideoHost,
        SerialManager::ReservedOutClientPortsEnum::VideoClient,
        dataLen,
        data
    );

    _Free(data);
    manager->SendPacket(packet);
}