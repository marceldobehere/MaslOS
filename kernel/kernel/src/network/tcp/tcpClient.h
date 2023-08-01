#pragma once
#include <stddef.h>
#include <stdint.h>
#include "../../serialManager/serialManager.h"


namespace TcpClient
{
    extern List<uint16_t>* connectedPorts;
    extern List<int>* receivedPacketsRemainingCount;
    extern List<SerialManager::GenericPacket*>* receivedPackets;
    extern bool InitDone;
    void InitTcpClientStuff();

    bool IsPacketValid(SerialManager::GenericPacket* packet);
    bool IsPacketForPort(SerialManager::GenericPacket* packet, uint16_t port);
    int GetPacketSize(SerialManager::GenericPacket* packet);
    void DoStep();
    void HandleCommandPacket(SerialManager::GenericPacket* packet);

    void AddPacket(SerialManager::GenericPacket* packet);
    void RemovePacketAt(int index);

    int DataAvaiableCount(uint16_t port);
    bool SendData(uint16_t port, int len, char* data);
    int ReadData(uint16_t port, int len, char* buff);

    bool IsPortConnected(uint16_t port);
    bool ConnectPortToIp(uint16_t portFrom, int ipTo, uint16_t portTo);
    void DisconnectPort(uint16_t port);
}
