#pragma once

#include <utility>
#include <memory>
#include <vector>
#include "CommandVersion.h"

class Command;
class CommandParameterData;
class CommandOrigin;
enum class CommandPermissionLevel;
enum class CommandFlag;

class CommandRegistry {

public:
    class ParseToken;
    /*struct Overload {
        using FactoryFn = std::unique_ptr<Command> (*)(void);
        CommandVersion version;
        std::unique_ptr<Command> (*allocator)();
        std::vector<CommandParameterData> params;
        int filler; // I don't think this is an actual member, looks like padding to me

        //Overload(CommandVersion version, std::unique_ptr<Command> (*allocator)()) : version(version),
        Overload(CommandVersion, FactoryFn);//                                                                      allocator(allocator) {}
    };*/
    struct Overload {
    using FactoryFn = std::unique_ptr<Command> (*)(void);
    CommandVersion version;                   // 0
    FactoryFn factory;                        // 8
    std::vector<CommandParameterData> params; // 16
    char unk;                                 // 40
    inline Overload(CommandVersion version, FactoryFn factory, std::vector<CommandParameterData> &&args)
        : version(version), factory(factory), params(std::move(args)), unk(0xFF) {}
  };
    struct Signature {
       std::string name;                                 // 0
       std::string desc;                                 // 32
        std::vector<CommandRegistry::Overload> overloads;
    };

    Signature* findCommand(std::string const&);

    void buildOverload(Overload&);

    void registerOverloadInternal(Signature&, Overload&);

    void registerCommand(std::string const&, char const*, CommandPermissionLevel, CommandFlag, CommandFlag);

    template <typename T>
    bool parse(void*, ParseToken const&, CommandOrigin const&, int, std::string&, std::vector<std::string, std::allocator<std::string>> &) const;

    /*template <typename T>
    static std::unique_ptr<Command> allocateCommand() {
        return std::unique_ptr<Command>(new T());
    }*/

    /*template <typename T, typename... Args>
    void registerOverload(const char* name, CommandVersion version, Args&&... args) {
        Signature* signature = findCommand(name);
        signature->overloads.emplace_back (version, allocateCommand<T>);
        Overload& overload = *signature->overloads.rbegin();
        //buildOverload(overload);
        overload.params = {args...};
        registerOverloadInternal(*signature, overload);
    }*/
    template <typename T> inline static std::unique_ptr<Command> allocateCommand() { return std::make_unique<T>(); }
  inline void registerOverload(
      std::string const &name, Overload::FactoryFn factory, std::vector<CommandParameterData> &&args) {
    Signature *signature = const_cast<Signature *>(findCommand(name));
    Overload overload(CommandVersion{}, factory, std::move(args));
    signature->overloads.push_back(overload);
    //auto &overload       = signature->overloads.emplace_back(CommandVersion{}, factory, std::move(args));
    registerOverloadInternal(*signature, overload);
  }
  template <typename T, typename... Params> inline void registerOverload(std::string const &name, Params... params) {
    registerOverload(name, &allocateCommand<T>, {params...});
  }

};