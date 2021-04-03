#pragma once

#include "../Core/Packet.h"
#include "../Math/Vec3.h"
#include "../dll.h"

class MoveActorAbsolutePacket : public Packet {
public:
  enum struct Flags { teleport = 1, on_ground = 2 };
  Flags flags = Flags::teleport;
  Vec3 pos;
  char pitch = 0, head_yaw = 0, yaw = 0;

  inline ~MoveActorAbsolutePacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual StreamReadResult read(ReadOnlyBinaryStream &);
};