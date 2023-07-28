#include "serialManager.h"
#include "../cStdLib/cStdLib.h"
#include "../devices/serial/serial.h"

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
        packetsToBeSent = new List<GenericPacket*>(100);
        packetsReceived = new List<GenericPacket*>(100);

        currentSendPacket = NULL;
        sendBuffer = new List<char>(100);

        receiveBuffer = new List<char>(100);
        receiveBufferLen = 0;

        clientConnected = false;
        clientCheckIndex = 0;
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

    void Manager::SendPacket(GenericPacket* packet)
    {
        AddToStack();

        AddToStack();
        bool hasToBeSentOut = false;
        for (int i = 0; i < ReservedOutClientPortLen; i++)
            if (packet->to == ReservedOutClientPorts[i])
            {
                hasToBeSentOut = true;
                break;
            }
        RemoveFromStack();

        if (hasToBeSentOut)
        {
            AddToStack();
            packetsToBeSent->add(packet);
            RemoveFromStack();
        }
        else
        {
            AddToStack();
            packetsReceived->add(packet);
            RemoveFromStack();
        }
        RemoveFromStack();
    }

    bool Manager::HasPacket(uint16_t to)
    {
        for (int t = 0; t < packetsReceived->getCount(); t++)
            if (packetsReceived->elementAt(t)->to == to)
                return true;
            
        return false;
    }

    GenericPacket* Manager::GetPacket(uint16_t to)
    {
        AddToStack();
        for (int t = 0; t < packetsReceived->getCount(); t++)
            if (packetsReceived->elementAt(t)->to == to)
            {
                GenericPacket* temp = packetsReceived->elementAt(t);
                packetsReceived->removeAt(t);

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
            packetsToBeSent->clear();
            RemoveFromStack();
            return false;
        }

        if (currentSendPacket == NULL && packetsToBeSent->getCount() > 0)
        {
            currentSendPacket = packetsToBeSent->elementAt(0);
            packetsToBeSent->removeAt(0);
            sendBuffer->clear();

            for (int i = 0; i < SignatureLen; i++)
                sendBuffer->add(Signature[i]);

            sendBuffer->add(currentSendPacket->type);

            {
                uint8_t* len = (uint8_t*)&currentSendPacket->len;
                for (int i = 0; i < 4; i++)
                    sendBuffer->add(len[i]);
            }

            {
                uint8_t* from = (uint8_t*)&currentSendPacket->from;
                for (int i = 0; i < 2; i++)
                    sendBuffer->add(from[i]);
            }

            {
                uint8_t* to = (uint8_t*)&currentSendPacket->to;
                for (int i = 0; i < 2; i++)
                    sendBuffer->add(to[i]);
            }

            for (int i = 0; i < currentSendPacket->len; i++)
                sendBuffer->add(currentSendPacket->data[i]);
        }
        if (currentSendPacket == NULL)
        {
            RemoveFromStack();
            return false;
        }
        
        if (sendBuffer->getCount() > 0)
        {
            char c = sendBuffer->elementAt(0);
            sendBuffer->removeAt(0);
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
        receiveBuffer->add(c);

        // if (receiveBuffer->getCount() < SignatureLen)
        // {
        //     RemoveFromStack();
        //     return true;
        // }

        if (receiveBuffer->getCount() > 0 && receiveBuffer->getCount() <= SignatureLen)
        {
            int recLen = receiveBuffer->getCount();
            bool signatureOk = true;
            for (int i = 0; i < recLen; i++)
                if (receiveBuffer->elementAt(i) != Signature[i])
                    signatureOk = false;
                    
            if (!signatureOk)
            {
                // we create a send packet and route it to the serial port

                char* tBuff = (char*)_Malloc(recLen);
                for (int i = 0; i < recLen; i++)
                    tBuff[i] = receiveBuffer->elementAt(i);

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
                

                receiveBuffer->clear();
            }

            RemoveFromStack();
            return true;
        }


        if (receiveBuffer->getCount() < SignatureLen + 5)
        {
            RemoveFromStack();
            return true;
        }


        if (receiveBuffer->getCount() == SignatureLen + 5)
        {
            char tempBuff[4] = {0, 0, 0, 0};
            for (int i = 0; i < 4; i++)
                tempBuff[i] = receiveBuffer->elementAt(SignatureLen + i + 1);

            int tempLen = *((int*)tempBuff) + 9 + SignatureLen;

            //Panic("LEN: {}", to_string(tempLen), true);
            if (tempLen > 1000) // for now
                tempLen = 9 + SignatureLen;
            if (tempLen < 9 + SignatureLen)
                tempLen = 9 + SignatureLen;

            receiveBufferLen = tempLen;
            RemoveFromStack();
            return true;
        }


        if (receiveBuffer->getCount() < receiveBufferLen)
        {
            RemoveFromStack();
            return true;
        }



        if (receiveBuffer->getCount() == receiveBufferLen)
        {
            char* tBuff = (char*)_Malloc(receiveBufferLen);
            for (int i = 0; i < receiveBufferLen; i++)
                tBuff[i] = receiveBuffer->elementAt(i);

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
            
            receiveBuffer->clear();

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

        AddToStack();
        
        CheckForClient();

        if (!clientConnected)
        {
            RemoveFromStack();
            return;
        }

        AddToStack();
        for (int i = 0; i < 10; i++)
            if (!DoReceiveStuff())
                break;
        RemoveFromStack();

        // basically echo
        if (Serial::CanRead())
        {
            char tempBuf[100];
            int i = 0;
            for (; i < 50 && Serial::CanRead(); i++)
            {
                char chr = Serial::Read();
                tempBuf[i] = chr;
            }
            tempBuf[i] = 0;
            Serial::Write(tempBuf);
        }

        AddToStack();
        for (int i = 0; i < 10; i++)
            if (!DoSendStuff())
                break;
        RemoveFromStack();

        RemoveFromStack();
    }
}