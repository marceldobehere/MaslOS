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
    pixelUpdates = new List<SerialPixelUpdate>(5000);
}

void SerialManagerDisplay::StartFrame()
{
    pixelUpdates->Clear();
}

void SerialManagerDisplay::UpdatePixel(int x, int y)
{
    if (x < 0 || y < 0 || x > framebuffer->Width || y > framebuffer->Height)
        return;
    
    pixelUpdates->Add(SerialPixelUpdate(x, y));

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

    // Structure will be
    // Line X (short) 0-1
    // Line Y (short) 2-3
    // Pixel Count (short) 4-5
    // Pixels (3 bytes each) 6-...

    AddToStack();
    int dataLen = 0;
    int c = pixelUpdates->GetCount();
    {
        int lastY = -1;
        int lastX = 0;
        for (int i = 0; i < c; i++)
        {
            SerialPixelUpdate upd = pixelUpdates->ElementAt(i);

            if (lastY != upd.y || upd.x != lastX + 1)
                dataLen += 6; // 6 bytes for line header
            
            lastX = upd.x;
            lastY = upd.y;

            dataLen += 3; // 3 bytes for pixel
        }
    }
    RemoveFromStack();


    //int dataLen = pixelUpdates->GetCount() * 7; (OLD WAY)

    //Panic("YES: {}", to_string(dataLen), true);

    AddToStack();

    AddToStack();
    uint8_t* data = (uint8_t*)_Malloc(dataLen);
    RemoveFromStack();

    {
        int dataIndex = 0;
        for (int i = 0; i < c; i++)
        {
            SerialPixelUpdate upd = pixelUpdates->ElementAt(i);

            data[dataIndex++] = (uint8_t)(upd.x & 0xFF);
            data[dataIndex++] = (uint8_t)((upd.x >> 8) & 0xFF);
            
            data[dataIndex++] = (uint8_t)(upd.y & 0xFF);
            data[dataIndex++] = (uint8_t)((upd.y >> 8) & 0xFF);

            int tempIndex = dataIndex;
            dataIndex += 2; // skip pixel count for now

            // get Pixel in the same line/area
            short tempPixelsInLine = 0;
            int lastX = upd.x - 1;
            for(; i < c; i++)
            {
                SerialPixelUpdate upd2 = pixelUpdates->ElementAt(i);
                if (upd2.y == upd.y && upd2.x == lastX + 1)
                {
                    tempPixelsInLine++;
                    uint32_t col = *(((uint32_t*)framebuffer->BaseAddress) + upd2.x + upd2.y * framebuffer->PixelsPerScanLine);

                    data[dataIndex++] = (uint8_t)((col >> 16) & 0xFF);
                    data[dataIndex++] = (uint8_t)((col >> 8) & 0xFF);
                    data[dataIndex++] = (uint8_t)(col & 0xFF);
                }
                else
                    break;

                lastX = upd2.x;
            }
            i--;

            int tempIndex2 = dataIndex;
            dataIndex = tempIndex;
            data[dataIndex++] = (uint8_t)(tempPixelsInLine & 0xFF);
            data[dataIndex++] = (uint8_t)((tempPixelsInLine >> 8) & 0xFF);
            dataIndex = tempIndex2;
        }
    }
    RemoveFromStack();

    // while (pixelUpdates->GetCount() > 0)
    // {
    //     SerialPixelUpdate update = pixelUpdates->Dequeue();
    //     uint32_t col = *(((uint32_t*)framebuffer->BaseAddress) + update.x + update.y * framebuffer->PixelsPerScanLine);

    //     data[dataIndex++] = (uint8_t)(update.x & 0xFF);
    //     data[dataIndex++] = (uint8_t)((update.x >> 8) & 0xFF);
        
    //     data[dataIndex++] = (uint8_t)(update.y & 0xFF);
    //     data[dataIndex++] = (uint8_t)((update.y >> 8) & 0xFF);

    //     // COL IS ARGB
    //     data[dataIndex++] = (uint8_t)((col >> 16) & 0xFF);
    //     data[dataIndex++] = (uint8_t)((col >> 8) & 0xFF);
    //     data[dataIndex++] = (uint8_t)(col & 0xFF);
    
    // }

    AddToStack();
    SerialManager::GenericPacket* packet = new 
    SerialManager::GenericPacket(
        SerialManager::PacketType::DATA,
        SerialManager::ReservedHostPortsEnum::VideoHost,
        SerialManager::ReservedOutClientPortsEnum::VideoClient,
        dataLen,
        data,
        false
    );
    RemoveFromStack();


    manager->SendPacket(packet);
}