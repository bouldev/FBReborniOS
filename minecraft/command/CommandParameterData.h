#pragma once

#include <string>
#include <vector>
#include "CommandRegistry.h"
#include "../util/typeid.h"

enum class CommandParameterDataType;

class CommandParameterData {

private:
    //char filler[0x28];

public:
    //CommandParameterData(typeid_t<CommandRegistry>, bool (CommandRegistry::*)(void*, CommandRegistry::ParseToken const&, CommandOrigin const&, int, std::string&, std::vector<std::string, std::allocator<std::string>> &) const, char const*, CommandParameterDataType, char const*, int, bool, int);
    //CommandParameterData(CommandParameterData const&);

};