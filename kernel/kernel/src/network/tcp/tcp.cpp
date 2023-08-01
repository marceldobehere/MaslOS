#include "tcpClient.h"
#include "../../osData/osData.h"
#include "../../devices/serial/serial.h"


namespace TcpClient
{
    List<uint16_t>* connectedPorts;
    List<int>* receivedPacketsRemainingCount = NULL;
    List<SerialManager::GenericPacket*>* receivedPackets = NULL;
    bool InitDone;
    void InitTcpClientStuff()
    {
        connectedPorts = new List<uint16_t>(20);
        receivedPacketsRemainingCount = new List<int>(50);
        receivedPackets = new List<SerialManager::GenericPacket*>(50);
        InitDone = true;
    }

    void AddPacket(SerialManager::GenericPacket* packet)
    {
        if (!InitDone)
            return;
        uint16_t port = *((uint16_t*)(packet->data + 1));
        //Serial::Writeln("> DATA PORT: {}", to_string(port));
        receivedPackets->Add(packet);
        receivedPacketsRemainingCount->Add(GetPacketSize(packet));
    }

    void RemovePacketAt(int index)
    {
        if (!InitDone)
            return;
        if (index < 0 || index >= receivedPackets->GetCount())
            return;
        receivedPackets->RemoveAt(index);
        receivedPacketsRemainingCount->RemoveAt(index);
    }

    #define PACKET_H_CS 0
    #define PACKET_H_PORT 1
    #define PACKET_C_IP 3
    #define PACKET_C_PORT 7
    #define PACKET_LEN 9
    #define PACKET_DATA 13

    // NORMAL DATA TYPE

    // Packet SEND Header 
    // CLIENT/SERVER 0-0
    // PORT TO 1-2
    // IP TO 3-6
    // PORT TO 7-8
    // LEN 9-12
    // DATA 13-...

    // Packet Receive Header
    // CLIENT/SERVER 0-0
    // PORT FROM 1-2
    // IP FROM 3-6
    // PORT FROM 7-8
    // LEN 9-12
    // DATA 13-...

    bool IsPacketValid(SerialManager::GenericPacket* packet)
    {
        if (packet->type != SerialManager::PacketType::DATA)
            return false;
        if (packet->len <= 13)
            return false;
        if (packet->data[PACKET_H_CS] != 0 && packet->data[PACKET_H_CS] != 1)
            return false;
        
        return true;
    }

    bool IsPacketForPort(SerialManager::GenericPacket* packet, uint16_t port)
    {
        return (*((uint16_t*)(packet->data + PACKET_H_PORT)) == port);
    }

    int GetPacketSize(SerialManager::GenericPacket* packet)
    {
        return *((int*)(packet->data + PACKET_LEN));
    }

    void DoStep()
    {
        if (!InitDone)
            return;
        if (osData.serialManager == NULL)
            return;
        
        while (osData.serialManager->HasPacket(SerialManager::ReservedHostPortsEnum::TCPHost))
        {
            SerialManager::GenericPacket* packet = osData.serialManager->GetPacket(SerialManager::ReservedHostPortsEnum::TCPHost);
            if (packet == NULL)
                continue;
            if (packet->type == SerialManager::PacketType::CMD)
            {
                //Serial::Writeln("> GOT CMD PACKET");
                HandleCommandPacket(packet);
                packet->Free();
                continue;
            }
            if (!IsPacketValid(packet))
            {
                //Serial::Writeln("> GOT INVALID PACKET");
                packet->Free();
                continue;
            }

            //Serial::Writeln("> GOT DATA PACKET");
            AddPacket(packet);
        }
    }

    // CMD PACKET
    // TYPE 0
    // PORT 1-2
    // EXTERNAL IP 3-6
    // EXTERNAL PORT 7-8
    // DATA 9-...

    #define CMD_PACKET_TYPE 0
    #define CMD_PACKET_PORT 1
    #define CMD_PACKET_EXT_IP 3
    #define CMD_PACKET_EXT_PORT 7
    #define CMD_PACKET_DATA 9

    #define CMD_PACKET_TYPE_CONNECTION 10
    

    void HandleCommandPacket(SerialManager::GenericPacket* packet)
    {
        if (packet->len < 9)
            return;
        
        if (packet->data[CMD_PACKET_TYPE] == 10)
        {
            //Serial::Writeln("> LEN: {}", to_string((int)packet->len));
            if (packet->len < 10)
                return;
            uint16_t port = *((uint16_t*)(packet->data + CMD_PACKET_PORT));
            //Serial::Writeln("> PORT: {}", to_string((int)port));
            bool connected = ((*((bool*)(packet->data + CMD_PACKET_DATA))));
            //Serial::Writeln("> CON: {}", to_string(connected));
            if (connected)
            {
                if (!IsPortConnected(port))
                    connectedPorts->Add(port);
            }
            else
            {
                if (IsPortConnected(port))
                    connectedPorts->RemoveAt(connectedPorts->GetIndexOf(port));
            }
        }
    }

    int DataAvaiableCount(uint16_t port)
    {
        if (!InitDone)
            return 0;
        
        int sum = 0;
        for (int i = 0; i < receivedPackets->GetCount(); i++)
        {
            SerialManager::GenericPacket* packet = receivedPackets->ElementAt(i);
            if (!IsPacketForPort(packet, port))
                continue;
            
            sum += receivedPacketsRemainingCount->ElementAt(i);
        }
        return sum;
    }

    bool SendData(uint16_t port, int len, char* data)
    {
        if (!InitDone)
            return false;
        if (osData.serialManager == NULL)
            return false;

        char* dataCopy = (char*)_Malloc(len + 13);

        dataCopy[0] = 0;
        *((uint16_t*)(dataCopy+1)) = port;
        *((int*)(dataCopy+3)) = 0; // ext IP
        *((uint16_t*)(dataCopy+7)) = 0; // ext Ports
        *((uint16_t*)(dataCopy+9)) = len;
        _memcpy(data, dataCopy + 13, len);

        SerialManager::GenericPacket* packet = new SerialManager::GenericPacket
        (
            SerialManager::PacketType::DATA,
            SerialManager::ReservedHostPortsEnum::TCPHost,
            SerialManager::ReservedOutClientPortsEnum::TCPClient,
            len + 13, 
            (uint8_t*)dataCopy,
            false
        );

        osData.serialManager->SendPacket(packet);
    }

    int ReadData(uint16_t port, int len, char* buff)
    {
        if (!InitDone)
            return 0;
        int left = len;
        int indx = 0;
        for (int i = 0; i < receivedPackets->GetCount(); i++)
        {
            SerialManager::GenericPacket* packet = receivedPackets->ElementAt(i);
            if (!IsPacketForPort(packet, port))
                continue;
            
            int canRead = receivedPacketsRemainingCount->ElementAt(i);
            int packetIndex = GetPacketSize(packet) - canRead + PACKET_DATA;
            for (int x = 0; x < canRead; x++)
            {
                buff[indx] = packet->data[packetIndex++];

                indx++;
                left--;
                if (left < 1)
                    return indx;   
            }

            RemovePacketAt(i);
            i--;
        }
    }

    bool IsPortConnected(uint16_t port)
    {
        if (!InitDone)
            return false;
        if (osData.serialManager == NULL)
            return false;
        
        return connectedPorts->GetIndexOf(port) != -1;
    }

    bool ConnectPortToIp(uint16_t portFrom, int ipTo, uint16_t portTo)
    {
        if (!InitDone)
            return false;
        if (osData.serialManager == NULL)
            return false;

        char* data = (char*)_Malloc(10);
        *((uint8_t*)(data + CMD_PACKET_TYPE)) = CMD_PACKET_TYPE_CONNECTION;
        *((uint16_t*)(data + CMD_PACKET_PORT)) = portFrom;
        *((int*)(data + CMD_PACKET_EXT_IP)) = ipTo;
        *((uint16_t*)(data + CMD_PACKET_EXT_PORT)) = portTo;
        *((bool*)(data + CMD_PACKET_DATA)) = true;

        SerialManager::GenericPacket* packet = new SerialManager::GenericPacket
        (
            SerialManager::PacketType::CMD,
            SerialManager::ReservedHostPortsEnum::TCPHost,
            SerialManager::ReservedOutClientPortsEnum::TCPClient,
            10, 
            (uint8_t*)data,
            false
        );

        osData.serialManager->SendPacket(packet);
    }

    void DisconnectPort(uint16_t port)
    {
        if (!InitDone)
            return;
        if (osData.serialManager == NULL)
            return;
        
        char* data = (char*)_Malloc(10);
        *((uint8_t*)(data + CMD_PACKET_TYPE)) = CMD_PACKET_TYPE_CONNECTION;
        *((uint16_t*)(data + CMD_PACKET_PORT)) = port;
        *((int*)(data + CMD_PACKET_EXT_IP)) = 0;
        *((uint16_t*)(data + CMD_PACKET_EXT_PORT)) = 0;
        *((bool*)(data + CMD_PACKET_DATA)) = false;

        SerialManager::GenericPacket* packet = new SerialManager::GenericPacket
        (
            SerialManager::PacketType::CMD,
            SerialManager::ReservedHostPortsEnum::TCPHost,
            SerialManager::ReservedOutClientPortsEnum::TCPClient,
            10, 
            (uint8_t*)data,
            false
        );

        osData.serialManager->SendPacket(packet);
    }
}