#include "serialManager.h"
#include "../cStdLib/cStdLib.h"
#include "../devices/serial/serial.h"
#include "../display/serialManagerDisplay/serialManagerDisplay.h"

namespace SerialManager
{
    GenericPacket::GenericPacket(PacketType type, uint16_t from, uint16_t to, int len, uint8_t* data)
    {
        this->type = type;
        this->from = from;
        this->to = to;
        this->len = len;
        this->data = (char*)_Malloc(len);
        _memcpy(data, this->data, len);
    }

    void GenericPacket::Free()
    {
        _Free(data);
        _Free(this);
    }


    Manager::Manager()
    {
        AddToStack();
        packetsToBeSent = new Queue<GenericPacket*>(100);
        packetsReceived = new List<GenericPacket*>(100);
        InInt = false;

        currentSendPacket = NULL;
        sendBuffer = new Queue<char>(5000);

        receiveBuffer = new List<char>(100);
        receiveBufferLen = 0;

        clientConnected = false;
        clientCheckIndex = 0;

        // just for testing
        // osData.currentDisplay = new SerialManagerDisplay(this, osData.currentDisplay->framebuffer);
        RemoveFromStack();
    }

    void Manager::CheckForClient()
    {
        if (clientConnected)
            return;

        if (!Serial::_CanRead())
            return;

        AddToStack();
        char c = Serial::_Read();
        if (c == Signature[clientCheckIndex])
        {
            clientCheckIndex++;
            if (clientCheckIndex == SignatureLen)
            {
                clientConnected = true;
                clientCheckIndex = 0;

                InitClientStuff();
            }
            RemoveFromStack();
            return;
        }
        else
        {
            clientCheckIndex = 0;
            //receiveBuffer->add(c); // for serial stuff yes
            Serial::_Write(c);
        }
        RemoveFromStack();
    }

    void Manager::InitClientStuff()
    {
        AddToStack();
        // write signature
        for (int i = 0; i < SignatureLen; i++)
            Serial::_Write(Signature[i]);
        
        uint8_t ON = 1;
        uint8_t OFF = 0;

        // send enabled serial port
        SendPacket(new GenericPacket(
            PacketType::STATE,
            ReservedHostPortsEnum::RawSerial,
            ReservedOutClientPortsEnum::RawSerialClient,
            1,
            &ON
        ));

        // send enabled video
        SendPacket(new GenericPacket(
            PacketType::STATE,
            ReservedHostPortsEnum::VideoHost,
            ReservedOutClientPortsEnum::VideoClient,
            1,
            &ON
        ));
        osData.currentDisplay = new SerialManagerDisplay(this, osData.currentDisplay->framebuffer);
        GlobalRenderer->Clear(Colors.black);
        // osData.windowPointerThing->Clear(true);
        // osData.windowPointerThing->RenderWindows();

        // write hoi packet
        Serial::Write("\nConnection to MaslOS established!\n");
        RemoveFromStack();
    }

    void Manager::SendPacket(uint16_t from, uint16_t to, GenericPacket* packet)
    {
        AddToStack();
        packet->from = from;
        packet->to = to;
        SendPacket(packet);
        RemoveFromStack();
    }

    bool Manager::HasPacketToBeSentOut(GenericPacket* packet)
    {
        AddToStack();
        bool hasToBeSentOut = false;
        for (int i = 0; i < ReservedOutClientPortLen; i++)
            if (packet->to == ReservedOutClientPorts[i])
            {
                hasToBeSentOut = true;
                break;
            }
        RemoveFromStack();
        return hasToBeSentOut;
    }

    bool Manager::CanPacketBeSent(bool sentOut, GenericPacket* packet)
    {
        bool canPacketBeSent = true;
        if (sentOut)
        {
            for (int i = 0; i < ReservedOutClientPortLen; i++)
                if (ReservedOutClientPorts[i] == packet->to)
                {
                    canPacketBeSent = WorkingOutClientPorts[i];
                    break;
                }
        }
        else
        {
            for (int i = 0; i < ReservedHostPortLen; i++)
                if (ReservedHostPorts[i] == packet->to)
                {
                    canPacketBeSent = WorkingHostPorts[i];
                    break;
                }
        }

        return canPacketBeSent || packet->type == PacketType::STATE;
    }

    void Manager::SendPacket(GenericPacket* packet)
    {
        AddToStack();
        bool hasToBeSentOut = HasPacketToBeSentOut(packet);

        if (packet->type == PacketType::STATE)
        {
            bool work = packet->data[0] == 1;

            for (int i = 0; i < ReservedOutClientPortLen; i++)
                if (ReservedOutClientPorts[i] == packet->from)
                {
                    WorkingOutClientPorts[i] = work;
                    break;
                }

            for (int i = 0; i < ReservedHostPortLen; i++)
                if (ReservedHostPorts[i] == packet->from)
                {
                    WorkingHostPorts[i] = work;
                    break;
                }

            if (!hasToBeSentOut)
            {
                packet->Free();
                RemoveFromStack();
                return;
            }
        }

        bool canPacketBeSent = CanPacketBeSent(hasToBeSentOut, packet);

        if (!canPacketBeSent)
        {
            packet->Free();
            RemoveFromStack();
            return;
        }

        if (hasToBeSentOut)
        {
            AddToStack();
            packetsToBeSent->Enqueue(packet);
            RemoveFromStack();
        }
        else
        {
            AddToStack();
            packetsReceived->Add(packet);
            RemoveFromStack();
        }
        RemoveFromStack();
    }

    bool Manager::HasPacket(uint16_t to)
    {
        for (int t = 0; t < packetsReceived->GetCount(); t++)
            if (packetsReceived->ElementAt(t)->to == to)
                return true;
            
        return false;
    }

    GenericPacket* Manager::GetPacket(uint16_t to)
    {
        AddToStack();
        for (int t = 0; t < packetsReceived->GetCount(); t++)
            if (packetsReceived->ElementAt(t)->to == to)
            {
                GenericPacket* temp = packetsReceived->ElementAt(t);
                packetsReceived->RemoveAt(t);

                RemoveFromStack();
                return temp;
            }
            
        RemoveFromStack();
        return NULL;
    }

    bool Manager::DoSendStuff()
    {
        AddToStack();
        if (!clientConnected)
        {
            packetsToBeSent->Clear();
            RemoveFromStack();
            return false;
        }

        if (currentSendPacket == NULL && packetsToBeSent->GetCount() > 0)
        {
            currentSendPacket = packetsToBeSent->Dequeue();
            sendBuffer->Clear();

            bool canPacketBeSent = CanPacketBeSent(true, currentSendPacket);
            if (!canPacketBeSent)
            {
                currentSendPacket->Free();
                currentSendPacket = NULL;
                RemoveFromStack();
                return true;
            }

            for (int i = 0; i < SignatureLen; i++)
                sendBuffer->Enqueue(Signature[i]);

            sendBuffer->Enqueue(currentSendPacket->type);

            {
                uint8_t* len = (uint8_t*)&currentSendPacket->len;
                for (int i = 0; i < 4; i++)
                    sendBuffer->Enqueue(len[i]);
            }

            {
                uint8_t* from = (uint8_t*)&currentSendPacket->from;
                for (int i = 0; i < 2; i++)
                    sendBuffer->Enqueue(from[i]);
            }

            {
                uint8_t* to = (uint8_t*)&currentSendPacket->to;
                for (int i = 0; i < 2; i++)
                    sendBuffer->Enqueue(to[i]);
            }

            for (int i = 0; i < currentSendPacket->len; i++)
                sendBuffer->Enqueue(currentSendPacket->data[i]);
        }
        if (currentSendPacket == NULL)
        {
            RemoveFromStack();
            return false;
        }
        
        if (sendBuffer->GetCount() > 0)
        {
            char c = sendBuffer->Dequeue();
            Serial::_Write(c);
        }
        else
        {
            currentSendPacket->Free();
            currentSendPacket = NULL;
        }

        RemoveFromStack();
        return true;
    }

    bool Manager::DoReceiveStuff()
    {
        if (!clientConnected)
            return false;

        if (!Serial::_CanRead())
            return false;

        AddToStack();
        char c = Serial::_Read();
        receiveBuffer->Add(c);

        // if (receiveBuffer->getCount() < SignatureLen)
        // {
        //     RemoveFromStack();
        //     return true;
        // }

        if (receiveBuffer->GetCount() > 0 && receiveBuffer->GetCount() <= SignatureLen)
        {
            int recLen = receiveBuffer->GetCount();
            bool signatureOk = true;
            for (int i = 0; i < recLen; i++)
                if (receiveBuffer->ElementAt(i) != Signature[i])
                    signatureOk = false;
                    
            if (!signatureOk)
            {
                // we create a send packet and route it to the serial port

                char* tBuff = (char*)_Malloc(recLen);
                for (int i = 0; i < recLen; i++)
                    tBuff[i] = receiveBuffer->ElementAt(i);

                GenericPacket* packet = new GenericPacket(
                    PacketType::DATA, 
                    ReservedOutClientPortsEnum::RawSerialClient, 
                    ReservedHostPortsEnum::RawSerial, 
                    recLen, 
                    (uint8_t*)tBuff
                );
                _Free(tBuff);

                AddToStack();
                SendPacket(packet);
                RemoveFromStack();
                

                receiveBuffer->Clear();
            }

            RemoveFromStack();
            return true;
        }


        if (receiveBuffer->GetCount() < SignatureLen + 5)
        {
            RemoveFromStack();
            return true;
        }


        if (receiveBuffer->GetCount() == SignatureLen + 5)
        {
            char tempBuff[4] = {0, 0, 0, 0};
            for (int i = 0; i < 4; i++)
                tempBuff[i] = receiveBuffer->ElementAt(SignatureLen + i + 1);

            int tempLen = *((int*)tempBuff) + 9 + SignatureLen;

            //Panic("LEN: {}", to_string(tempLen), true);
            if (tempLen > 10000000) // for now
                tempLen = 9 + SignatureLen;
            if (tempLen < 9 + SignatureLen)
                tempLen = 9 + SignatureLen;

            receiveBufferLen = tempLen;
            RemoveFromStack();
            return true;
        }


        if (receiveBuffer->GetCount() < receiveBufferLen)
        {
            RemoveFromStack();
            return true;
        }



        if (receiveBuffer->GetCount() == receiveBufferLen)
        {
            char* tBuff = (char*)_Malloc(receiveBufferLen);
            for (int i = 0; i < receiveBufferLen; i++)
                tBuff[i] = receiveBuffer->ElementAt(i);

            GenericPacket* packet = new GenericPacket(
                (PacketType)tBuff[SignatureLen], 
                *((uint16_t*)&tBuff[SignatureLen + 5]), 
                *((uint16_t*)&tBuff[SignatureLen + 7]), 
                receiveBufferLen - 9 - SignatureLen, 
                (uint8_t*)&tBuff[9 + SignatureLen]
            );
            _Free(tBuff);

            AddToStack();
            SendPacket(packet);
            RemoveFromStack();
            
            receiveBuffer->Clear();

            RemoveFromStack();
            return true;
        }

        RemoveFromStack();
        return true;
    }


    void Manager::DoStuff()
    {
        if (mallocToCache)
            return;

        if (InInt)
            return;
        InInt = true;

        AddToStack();
        
        CheckForClient();

        if (!clientConnected)
        {
            RemoveFromStack();
            InInt = false;
            return;
        }

        AddToStack();
        for (int i = 0; i < 224; i++)
            if (!DoReceiveStuff())
                break;
        RemoveFromStack();

        // // basically echo
        // if (Serial::CanRead())
        // {
        //     char tempBuf[100];
        //     int i = 0;
        //     for (; i < 50 && Serial::CanRead(); i++)
        //     {
        //         char chr = Serial::Read();
        //         tempBuf[i] = chr;
        //     }
        //     tempBuf[i] = 0;
        //     Serial::Write(tempBuf);
        // }

        AddToStack();
        for (int i = 0; i < 224; i++)
            if (!DoSendStuff())
                break;
        RemoveFromStack();

        RemoveFromStack();

        InInt = false;
    }
}