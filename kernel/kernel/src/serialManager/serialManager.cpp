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
        packetsToBeSent = new List<GenericPacket*>(100);
        packetsReceived = new List<GenericPacket*>(100);

        currentSendPacket = NULL;
        sendBuffer = new List<char>(100);

        receiveBuffer = new List<char>(100);
        receiveBufferLen = 0;

        clientConnected = false;
        clientCheckIndex = 0;
    }

    void Manager::CheckForClient()
    {
        if (clientConnected)
            return;

        if (!Serial::_CanRead())
            return;

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
            return;
        }
        else
        {
            clientCheckIndex = 0;
            //receiveBuffer->add(c); // for serial stuff yes
            Serial::_Write(c);
        }
    }

    void Manager::InitClientStuff()
    {
        // write signature
        for (int i = 0; i < SignatureLen; i++)
            Serial::_Write(Signature[i]);
        
        // write hoi packet
        Serial::Write("HOI!");
    }

    void Manager::SendPacket(uint16_t from, uint16_t to, GenericPacket* packet)
    {
        packet->from = from;
        packet->to = to;
        SendPacket(packet);
    }

    void Manager::SendPacket(GenericPacket* packet)
    {
        bool hasToBeSentOut = false;
        for (int i = 0; ReservedOutClientPortLen; i++)
            if (ReservedOutClientPorts[i] == packet->to)
            {
                hasToBeSentOut = true;
                break;
            }

        if (hasToBeSentOut)
            packetsToBeSent->add(packet);
        else
            packetsReceived->add(packet);
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
        for (int t = 0; t < packetsReceived->getCount(); t++)
            if (packetsReceived->elementAt(t)->to == to)
            {
                GenericPacket* temp = packetsReceived->elementAt(t);
                packetsReceived->removeAt(t);
                return temp;
            }
            
        return NULL;
    }

    bool Manager::DoSendStuff()
    {
        if (!clientConnected)
        {
            packetsToBeSent->clear();
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
                uint8_t* to = (uint8_t*)&currentSendPacket->to;
                for (int i = 0; i < 2; i++)
                    sendBuffer->add(to[i]);
            }

            {
                uint8_t* from = (uint8_t*)&currentSendPacket->from;
                for (int i = 0; i < 2; i++)
                    sendBuffer->add(from[i]);
            }

            for (int i = 0; i < currentSendPacket->len; i++)
                sendBuffer->add(currentSendPacket->data[i]);

        }
        if (currentSendPacket == NULL)
            return false;
        
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

        return true;
    }
    bool Manager::DoReceiveStuff()
    {
        if (!clientConnected)
            return false;

        if (!Serial::_CanRead())
            return false;

        char c = Serial::_Read();
        receiveBuffer->add(c);

        if (receiveBuffer->getCount() < SignatureLen)
            return true;

        if (receiveBuffer->getCount() == SignatureLen)
        {
            bool signatureOk = true;
            for (int i = 0; i < SignatureLen; i++)
                if (receiveBuffer->elementAt(i) != Signature[i])
                    signatureOk = false;
                    
            if (!signatureOk)
            {
                // we create a send packet and route it to the serial port

                char* tBuff = (char*)_Malloc(SignatureLen);
                for (int i = 0; i < SignatureLen; i++)
                    tBuff[i] = receiveBuffer->elementAt(i);

                GenericPacket* packet = new GenericPacket(
                    PacketType::DATA, 
                    ReservedOutClientPortsEnum::RawSerialClient, 
                    ReservedHostPortsEnum::RawSerial, 
                    SignatureLen, 
                    tBuff
                );

                SendPacket(packet);
                _Free(tBuff);

                receiveBuffer->clear();
            }
            return true;
        }


        if (receiveBuffer->getCount() < SignatureLen + 5)
            return true;


        if (receiveBuffer->getCount() == SignatureLen + 5)
        {
            char tempBuff[4] = {0, 0, 0, 0};
            for (int i = 0; i < 4; i++)
                tempBuff[i] = receiveBuffer->elementAt(SignatureLen + i);

            int tempLen = *((int*)tempBuff) + 6 + SignatureLen;

            //Panic("LEN: {}", to_string(tempLen), true);
            if (tempLen > 1000) // for now
                tempLen = 10;
            if (tempLen < 9)
                tempLen = 9;

            receiveBufferLen = tempLen;
            return true;
        }


        if (receiveBuffer->getCount() < receiveBufferLen)
            return true;



        if (receiveBuffer->getCount() == receiveBufferLen)
        {
            char* tBuff = (char*)_Malloc(receiveBufferLen);
            for (int i = 0; i < receiveBufferLen; i++)
                tBuff[i] = receiveBuffer->elementAt(i);

            GenericPacket* packet = new GenericPacket(
                (PacketType)tBuff[3], 
                *((uint16_t*)&tBuff[10]), 
                *((uint16_t*)&tBuff[8]), 
                receiveBufferLen - 6 - SignatureLen, 
                (uint8_t*)&tBuff[12]
            );

            SendPacket(packet);
            _Free(tBuff);

            receiveBuffer->clear();

            return true;
        }


        return true;
    }


    void Manager::DoStuff()
    {
        
        CheckForClient();

        if (!clientConnected)
            return;

        for (int i = 0; i < 10; i++)
            if (!DoReceiveStuff())
                break;

        for (int i = 0; i < 10; i++)
            if (!DoSendStuff())
                break;

    }
}