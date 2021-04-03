#pragma once

#include "../Core/Packet.h"
#include "../Core/ContainerID.h"
#include "../dll.h"

class ContainerSetDataPacket : public Packet {
public:
  enum struct Property {
    FuranceTickCount   = 0,
    FuranceLitTime     = 1,
    FuranceLitDuration = 2,
    FuranceLitStoredXP = 3,

    BrewingStandBrewTime   = 0,
    BrewingStandFuelAmount = 1,
    BrewingStandFuelTotal  = 2,
  };

  Property property;
  int value;
  ContainerID id = ContainerID::Invalid;

  inline ~ContainerSetDataPacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual StreamReadResult read(ReadOnlyBinaryStream &);
};