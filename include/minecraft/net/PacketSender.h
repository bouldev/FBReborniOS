#pragma once

#include <functional>

class Packet;
class NetworkIdentifier;

class PacketSender {

public:

    // virtual
    virtual ~PacketSender();
    virtual void* send(Packet&) = 0;
    virtual void* sendToServer(Packet&) = 0;
    virtual void* sendToClient(NetworkIdentifier const&, Packet const&, unsigned char) = 0;
    virtual void* sendBroadcast(Packet const&) = 0;
    virtual void* sendBroadcast(NetworkIdentifier const&, unsigned char, Packet const&) = 0;
    virtual void* flush(NetworkIdentifier const&, std::function<void ()>) = 0;

};