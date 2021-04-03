#pragma once

#include "../Core/Packet.h"
#include "../Actor/ActorUniqueID.h"
#include "../dll.h"

class RemoveActorPacket : public Packet {
public:
  ActorUniqueID uid;

  inline ~RemoveActorPacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual StreamReadResult read(ReadOnlyBinaryStream &);
};