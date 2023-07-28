#pragma once
#include <stdint.h>
#include <stddef.h>


namespace SerialManager
{
    class GenericPacket;
}

#include "../cStdLib/list/list_serialPacket.h"
#include "../cStdLib/list/list_basics.h"

namespace SerialManager
{
    #define SignatureLen 3
    static const char Signature[SignatureLen]
    {
        (char)6, '~', '\\' //(char)178, (char)234, (char)242
    };

    enum ReservedHostPortsEnum : uint16_t
    {
        InitClient = 001, 
        RawSerial = 101,
        VideoHost = 201,
        AudioHost = 301,
        HIDClient = 401,
        TCPHost = 501,
        FileShareClient = 601,
    };

    #define ReservedHostPortLen 7
    static const uint16_t ReservedHostPorts[ReservedHostPortLen]
    {
        ReservedHostPortsEnum::InitClient,
        ReservedHostPortsEnum::RawSerial,
        ReservedHostPortsEnum::VideoHost,
        ReservedHostPortsEnum::AudioHost,
        ReservedHostPortsEnum::HIDClient,
        ReservedHostPortsEnum::TCPHost,
        ReservedHostPortsEnum::FileShareClient
    }; // this is prolly not even needed, as I only need to check for outgoing packets i guess
    // but it defines which ports the clients/hosts will get data sent to

    enum ReservedOutClientPortsEnum : uint16_t
    {
        InitHost = 002,
        RawSerialClient = 102,
        VideoClient = 202,
        AudioClient = 302,
        HIDHost = 402,
        TCPClient = 502,
        FileShareHost = 602,
    };

    #define ReservedOutClientPortLen 7
    static const uint16_t ReservedOutClientPorts[ReservedOutClientPortLen]
    {
        ReservedOutClientPortsEnum::InitHost,
        ReservedOutClientPortsEnum::RawSerialClient,
        ReservedOutClientPortsEnum::VideoClient,
        ReservedOutClientPortsEnum::AudioClient,
        ReservedOutClientPortsEnum::HIDHost,
        ReservedOutClientPortsEnum::TCPClient,
        ReservedOutClientPortsEnum::FileShareHost
    }; 

    enum PacketType : uint8_t
    {
        STATE = 10,
        PING = 20,
        DATA = 30,
    };

    class GenericPacket
    {
    public:
        PacketType type;    // BYTE 0
        int len;          // BYTE 1-4
        uint16_t from;        // BYTE 5-6
        uint16_t to;           // BYTE 7-8
        char* data;         // BYTE 9-...

        uint64_t timeSent; // only used for timeout on the list

        GenericPacket(PacketType type, uint16_t from, uint16_t to, int len, uint8_t* data); // data will be copied into the buffer, so it will need to be freed later

        void Free();
    };


    // PACKET IN SERIAL STREAM:
    // SIGNATURE
    // 0-2: 0x61, 0x9A, 0xF1
    // TYPE
    // 3: 0x00 = ON, 0x01 = OFF, 0x02 = PING, 0x03 = DATA
    // LEN
    // 4-7: 32 bit unsigned int
    // TO
    // 8-9: 16 bit unsigned int
    // FROM
    // 10-11: 16 bit unsigned int
    // DATA
    // 12-...: len bytes of data

    class Manager
    {
    public:
        List<GenericPacket*>* packetsToBeSent = NULL; // to be sent out to serial
        List<GenericPacket*>* packetsReceived = NULL; // received from serial or locally
        
        GenericPacket* currentSendPacket = NULL;
        List<char>* sendBuffer = NULL;

        List<char>* receiveBuffer = NULL;
        int receiveBufferLen = 0;

        bool clientConnected = false; // if there is no client, no outbound stuff will work and also i will have to use the normal serial for like serial writes
        int clientCheckIndex = 0;
        void CheckForClient();

        Manager();
        void SendPacket(uint16_t from, uint16_t to, GenericPacket* packet);
        void SendPacket(GenericPacket* packet);
        bool HasPacket(uint16_t to);
        GenericPacket* GetPacket(uint16_t to);

        void DoStuff();
        bool DoSendStuff();
        bool DoReceiveStuff();
        
        void InitClientStuff();




        bool WorkingHostPorts[ReservedHostPortLen]
        {
            true,
            true,
            false,
            false,
            false,
            false,
            false
        };


        bool WorkingOutClientPorts[ReservedOutClientPortLen]
        {
            true,
            false,
            false,
            false,
            false,
            false,
            false
        }; 
    };
}