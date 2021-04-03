#include <unordered_map>

template <typename Type, typename Store> struct AutomaticID {
  Store v;
  template <typename X>
  AutomaticID(X v)
      : v(v) {}
  Store value() const;
  bool operator!=(AutomaticID const &) const;
  bool operator==(AutomaticID const &) const;
  operator Store() const { return v; }
  static Store _makeRuntimeID();
};

struct Dimension;
struct Biome;
using DimensionId = AutomaticID<Dimension, int>;
using BiomeId     = AutomaticID<Biome, int>;

class Vec3 {
public:
	float x,y,z;
};

class Actor {
public:
	void remove(void);
	void removeEntity(void);
	void kill(void);
	BlockSource *getRegion() const;
	void setCanFly(bool);
	Level* getLevel() const;
	Vec3 const &getPos() const;
	Vec3 const &getPosExtrapolated(float) const;
};

class Agent : public Actor {

};

class Scheduler {
public:
	Scheduler(std::string,uint);
};

class Player : public Actor {
public:
	bool isOperator() const;
	Agent* getAgent() const;
	void setAgent(Agent*);
	void changeDimensionWithCredits(DimensionId);
};

enum class GameType {
};

class LocalPlayer : public Player {
public:
	void setPlayerGameType(GameType);
};

class ServerPlayer : public Player {
public:
	void changeDimension(DimensionId,bool);
};

class Dimension{
public:
	Dimension(Level &, DimensionId,short,Scheduler &,std::string);
};class ChunkSource{};

namespace mce {
struct UUID {
  uint64_t most, least;

  UUID() {}

  UUID(const UUID &oth) {
    most  = oth.most;
    least = oth.least;
  }

  const std::string asString() const;

  static UUID fromStringFix(std::string const &str);

  bool operator==(UUID const &another) const;
  bool operator<(UUID const &another) const;
  bool operator!=(UUID const &another) const;
};
} // namespace mce

/*namespace std {
template <> struct hash<mce::UUID> {
  size_t operator()(mce::UUID const &uuid) const {
    auto seed = std::hash<unsigned long long>{}(uuid.most);
    seed ^= std::hash<unsigned long long>{}(uuid.least) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
  }
};*/

struct ActorUniqueID {
  int64_t data;

  static ActorUniqueID fromClientId(int64_t);
  static ActorUniqueID fromUUID(mce::UUID const &);
  size_t getHash() const;
  operator bool() const;
  operator int64_t() const;

  bool operator!=(ActorUniqueID const &) const;
  bool operator==(ActorUniqueID const &) const;
  bool operator<(ActorUniqueID const &) const;
  ActorUniqueID &operator++();
};

struct PlayerListEntry {
  ActorUniqueID id;             // 0
  mce::UUID uuid;               // 8
  std::string name;             // 24
  std::string skin;             // 56
  std::vector<char> skin_image; // 88
  std::vector<char> cape_image; // 112
  std::string skin_geo_name;    // 136
  std::string skin_geo_data;    // 168
  std::string xuid;             // 232
};

class Level{
public:
	char filler[0x96];
	Scheduler scheduler;
	Player getPlayer(mce::UUID const&) const;
	std::unordered_map<mce::UUID, PlayerListEntry> &getPlayerList();
	Dimension *createDimension(DimensionId);
};

class Color {
public:
	std::string toHexString(void) const;
};

class BlockLegacy;

class Block {
public:
	unsigned char filler[7];
	BlockLegacy **legacy_block;

	BlockLegacy const *getLegacyBlockPtr() const;
	char *getDescriptionId() const;
	std::string getRuntimeId() const;
	Color *getColor() const;
	Color& getMapColor() const;
	bool isContainerBlock() const;
};

typedef Block* (*createBlock)(unsigned char, void *);

struct Entity
{
  const std::string &getNameTag() const;
};

class Vec3;

template <typename T>
struct CommandSelectorResults
{
  std::shared_ptr<std::vector<T *>> content;
  bool empty() const;
  int size() const;
};

struct CommandSelectorBase
{
  CommandSelectorBase(bool);
  virtual ~CommandSelectorBase() {};
};

template <typename T>
struct CommandSelector : CommandSelectorBase
{
  char filler[0x74];
  CommandSelector();

  const CommandSelectorResults<T> results(CommandOrigin const &) const;
};

struct CommandSelectorPlayer : CommandSelector<Player>
{
  CommandSelectorPlayer() : CommandSelector() {}
  ~CommandSelectorPlayer() {}

  static typeid_t<CommandRegistry> type_id()
  {
    static typeid_t<CommandRegistry> ret = type_id_minecraft_symbol<CommandRegistry>("_ZZ7type_idI15CommandRegistry15CommandSelectorI6PlayerEE8typeid_tIT_EvE2id");
    return ret;
  }
};

struct CommandSelectorActor : CommandSelector<Actor>
{
  CommandSelectorActor() : CommandSelector() {}
  ~CommandSelectorActor() {}

  static typeid_t<CommandRegistry> type_id()
  {
    static typeid_t<CommandRegistry> ret = type_id_minecraft_symbol<CommandRegistry>("_ZZ7type_idI15CommandRegistry15CommandSelectorI5ActorEE8typeid_tIT_EvE2id");
    return ret;
  }
};

struct CommandPosition
{
  CommandPosition() {}
  ~CommandPosition() {}

  static typeid_t<CommandRegistry> type_id()
  {
    static typeid_t<CommandRegistry> ret = type_id_minecraft_symbol<CommandRegistry>("_ZZ7type_idI15CommandRegistry15CommandPositionE8typeid_tIT_EvE2id");
    return ret;
  }
};

class Options {
public:
	void setDevShowDevConsoleButton(bool);
};

struct GuiData{
	void showPopupNotice(std::string const&,std::string const&);
	void showTipMessage(std::string const&);
	void onAppSuspended();
};

class IClientInstance {};

class ScreenContext{
public:
	ScreenContext(void *things);
};

class CommandContext {
public:
	CommandContext(std::string const&, std::unique_ptr<CommandOrigin>, int);
};

struct HashType64 {
  std::int64_t value;
  static HashType64 NullHashType64;
  bool operator==(HashType64 const &) const;
};

struct HashedString {
  std::string str; // 0
  HashType64 hash; // 32
  static HashedString EMPTY_STRING;
  static HashType64 hash64(char const *);
  static HashType64 hash64(std::string const &);

  HashedString(HashedString const &);
  HashedString(HashedString &&);
  HashedString(char const *);
  HashedString(std::string const &);
  HashedString();

  HashedString &operator=(HashedString const &);
  HashedString &operator=(HashedString &&);
  bool operator==(HashedString const &);
  bool operator==(HashType64 const &);
  bool operator==(std::string const &);

  //virtual ~HashedString();
};

class MinecraftCommands {
public:
	void executeCommand(std::shared_ptr<CommandContext>,bool);
	void requestCommandExecution(std::unique_ptr<CommandOrigin>,std::string const&,int,bool) const;
};

enum class CommandOriginSystem{};
enum class CurrentCmdVersion{};

class ServerLevel : public Level {
public:
	MinecraftCommands *getCommands();
	void runCommand(HashedString const&,CommandOrigin&,CommandOriginSystem,CurrentCmdVersion);
};

class ActorBlockSyncMessage {};

class BlockSource {
public:
	Block *getBlock(int,int,int) const;
	void setBlock(BlockPos const&,Block const&,int/*3*/,ActorBlockSyncMessage const*/*nullptr*/);
	void setBlockNoUpdate(BlockPos const&,Block const&);
	void setExtraBlock(BlockPos const&,Block const&,int);
};

class Material {

};

class BlockPalette {

};

class TntBlock:public Block {
public:
	TntBlock(std::string const&,int);
};

class BlockLegacy {
public:
	BlockLegacy(std::string const&,int,Material const&);
	std::string getFullName() const;
	Block const* getStateFromLegacyData(unsigned short) const;
};

template <typename T> class SharedPtr {
  T **value;

public:
  SharedPtr(T *value);
  ~SharedPtr();
  template <typename... RS> static SharedPtr<T> make(RS &&...);
  void reset();
  T *const operator*() const;
  T *const operator->() const;
};

template <typename T> class WeakPtr {
public:
  T **ptr;

  WeakPtr(SharedPtr<T> const &);
  //void reset();
  //inline ~WeakPtr() { reset(); }
  bool isNull() const;
  //operator T *() const;
  //T *const operator*() const;
  //T *const operator->() const;
};

class BlockTypeRegistry {
public:
	static WeakPtr<BlockLegacy> lookupByName(std::string const &);
};

class GlobalPauseCommand : public Command {
public:
	static void setup(CommandRegistry& registry);
};

class GetEduClientInfoCommand : public Command {
public:
	//static void setup(CommandRegistry& registry,ActiveDirectoryIndentity const*,bool);
};

class GetChunksCommand : public Command {
public:
	static void setup(CommandRegistry& registry);
};

class GetChunkDataCommand : public Command {
public:
	static void setup(CommandRegistry& registry);
};

class GetSpawnPointCommand : public Command {
public:
	static void setup(CommandRegistry& registry);
};

class ClassRoomModeCommand : public Command {
public:
	static void setup(CommandRegistry& registry);
};

class CodeCommand : public Command {
public:
	static void setup(CommandRegistry& registry);
};

class Minecraft {
public:
	void setSimTimePause(bool);
};

class ServerCommand {

};

class ClientInstance {
public:
	GuiData *getGuiData() const;
	void onAppSuspended();
};

class AppPlatform {
public:
	void _fireAppFocusLost();
	void _fireAppFocusGained();
	void _fireAppSuspended();
	void _fireAppResumed();
	void _fireAppTerminated();
};

class ServerInstance;
class MinecraftGame {
public:
	void onAppSuspended();
	void onAppPreSuspended();
	void onAppUnpaused();
	void onAppPaused();
	void onAppResumed();
	void releaseMouse();
	void openPauseMenu();
	void onAppFocusLost();
	void onAppFocusGained();
	AppPlatform getApp();
	ServerInstance *getServerInstance();
};

class TextPacket {
public:
	static TextPacket createRaw(std::string const&);
	static TextPacket createSystemMessage(std::string const&);
};

class PlayerCommandOrigin {

public:

	// Virtual

	void setAllowHiddenCommands(bool);
	// Static

	// Field

};

class ServerCommandOrigin : public CommandOrigin {
public:
	ServerCommandOrigin(std::string const& s, ServerLevel& m,CommandPermissionLevel);
	//virtual ~CommandOrigin();
	virtual void getRequestId();
	virtual void getName();
	virtual void getBlockPosition();
	virtual void getWorldPosition();
	virtual Level* getLevel() const;
	virtual Dimension* getDimension();
	virtual Actor* getEntity() const;
	virtual void* getPermissionsLevel() const;
	virtual void* clone() const;
	virtual bool canCallHiddenCommands() const;
	virtual bool hasChatPerms() const;
	virtual bool hasTellPerms() const;
	virtual bool canUseAbility(std::string const&) const;
	virtual void* getSourceId() const;
	virtual void* getSourceSubId() const;
	virtual void* getOutputReceiver() const;
	virtual CommandOriginType getOriginType() const;
	virtual void* toCommandOriginData() const;
	virtual mce::UUID& getUUID() const;
	virtual void* _setUUID(mce::UUID const&);
	virtual bool getAreaAt(BlockPos const&) const;
};

class ServerInstance {
public:
	void queueForServerThread(std::function<void()>);
};

class CommandFilePath {
public:
	std::string getText();
};