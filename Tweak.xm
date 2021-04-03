#include "substrate.h"
#include <string>
#include <cstdio>
#include <mach-o/dyld.h>
#include <cstdint>
#include <stdint.h>
#include <vector>
#include <thread>

#import <Foundation/Foundation.h>
#import "UIKit/UIKit.h"

#import "MinecraftHeaders/Core/type_id.h"
#import "MinecraftHeaders/Command/CommandParameterData.h"
#import "MinecraftHeaders/Command/Command.h"
#import "MinecraftHeaders/Command/CommandOrigin.h"
#import "MinecraftHeaders/Command/CommandOutput.h"
#import "MinecraftHeaders/Math/BlockPos.h"

//#include "include/algorithms.h"
#include "FBE/fbios.h"

bool (*BEDROCK_Threading_ON_MAIN_THREAD)();

class MinecraftCommands {
public:
	CommandRegistry &getRegistry();
	void *requestCommandExecutionWhatever(std::string const& cmd,Player *ori,bool isfromplayer) const;
	void *requestCommandExecution(std::unique_ptr<CommandOrigin> origin,std::string const& cmd,int unk1,bool nooutput) const;
};

class Minecraft {
public:
	MinecraftCommands *getCommands();
};

//Minecraft *mGame;//Find from getlocalplayername
MinecraftCommands *mCmds;
std::unique_ptr<CommandOrigin> tempOrigin;

class ClientInstance {
public:
	//SceneFactory *getSceneFactory();
	//+1248: sub_1006A82B0 found,getMinecraft.
};

class GuiData {
public:
	//Can get from ClientInstanceScreenModel's executeCommand sub.
	void displaySystemMessage(std::string const &msg,std::string const& empty);
};

class MinecraftGame {
public:
	ClientInstance *getPrimaryClientInstance();
	GuiData *getPrimaryGuiData();
};

class MobileClient {
public:
	static MobileClient *singleton();
	MinecraftGame *getMinecraftGame();
	void performFuncInMainThread(std::function<void()> const&);
};

class CommandOutput;

extern void fastSetAddr(void** a,int64_t b);
#define fsa fastSetAddr

class PlayerCommandOrigin : public CommandOrigin {
public:
	PlayerCommandOrigin(Player &player);
};

class Player : public Actor {
public:
	~Player() override;
	void spawnExperienceOrb(Vec3 const& pos,int64_t val);
	bool add(ItemStack const&);
	bool drop(ItemStack const&,bool);
};

class LocalPlayer : public Player {

};

/*MinecraftCommands *getMCCommands(){
	return mGame->getCommands();
}*/

std::unique_ptr<FastBuilderiOS> globalFastBuilder;

class CommandMessage {
public:
	int64_t unk[3];
	std::string getMessage(CommandOrigin const& origin) const;
	CommandMessage(){
		unk[0]=0;
		unk[1]=0;
		unk[2]=0;
	}
};

template <typename Command, typename Type> int getOffset(Type Command::*src) {
  union {
    Type Command::*src;
    int value;
  } u;
  u.src = src;
  return u.value;
}

typeid_t<CommandRegistry> *cmdmsgtypeid;//saycommand
CommandParameterData::ParseFn cmdMsgParseFn;//Find in saycommand

template <typename Command, typename Type>
CommandParameterData mandatory(Type Command::*field, std::string name, bool Command::*isSet = nullptr) {
  return {
      *cmdmsgtypeid,//GetParameterTypeId<Type>(),
      cmdMsgParseFn,//CommandRegistry::getParseFn<Type>(),
      name,
      CommandParameterDataType::NORMAL,
      nullptr,
      getOffset(field),
      false,
      isSet ? getOffset(isSet) : -1,
  };
}

/*Vec3 fbpos;

class getposCommand : public Command {
public:
	~getposCommand() override=default;
	static void setup(CommandRegistry& registry){
		registry.registerCommand("fbgetpos","FB:Get pos",CommandPermissionLevel::Any,CommandFlagNone,CommandFlagCheat);
		registry.registerOverload<getposCommand>("fbgetpos");
	}

	void execute(CommandOrigin const& origin,CommandOutput& output) override {
		Player *player=(Player*)origin.getEntity();
		fbpos=player->getPos();
		char content[64];
		sprintf(content,"Done: %d,%d,%d",(int)fbpos.x,(int)fbpos.y,(int)fbpos.z);
		output.success(std::string(content));
		return;
	}
};*/

class FBCommand : public Command {
	CommandMessage FBCommandMsg;
public:
	
	~FBCommand() override=default;
	static void setup(CommandRegistry& registry){
		registry.registerCommand("fb","Run FastBuilder command",CommandPermissionLevel::Any,CommandFlagNone,CommandFlagCheat);
		registry.registerOverload<FBCommand>("fb",mandatory(&FBCommand::FBCommandMsg,"commandline"));
	}
	
	void execute(CommandOrigin const& origin,CommandOutput& output) override {
		tempOrigin=origin.clone();
		FBCommandReturn status=globalFastBuilder->executeFBCommand(FBCommandMsg.getMessage(origin));
		if(status==FBCommandReturn::OK){
			output.success("Building structure");
		}else if(status==FBCommandReturn::IsGetPos){
			Player *player=(Player*)origin.getEntity();
			Vec3 pos=player->getPos();
			pos.x-=0.5;
			pos.y--;
			pos.z-=0.5;
			globalFastBuilder->setPos(pos.x,pos.y,pos.z);
			char posgot[256];
			sprintf(posgot,"Got position: %d,%d,%d.",(int)pos.x,(int)pos.y,(int)pos.z);
			output.success(std::string(posgot));
		}else{
			output.error(FastBuilder::GetCommandReturn(status));
		}
	}
};

/*class buildRoundCommand : public Command {
public:
	~buildRoundCommand() override=default;
	static void setup(CommandRegistry& registry){
		registry.registerCommand("fbround","Build a round(test)",CommandPermissionLevel::Any,CommandFlagNone,CommandFlagCheat);
		registry.registerOverload<buildRoundCommand>("fbround");
	}

	void execute(CommandOrigin const& origin,CommandOutput& output) override {
		//Player *player=(Player*)origin.getEntity();
		session *roundSession=CoreBuildMethods::buildround(fbpos.x,fbpos.y,fbpos.z,'x',10);
		//MinecraftCommands *mcc=getMCCommands();
		char commandBuffer[512];
		std::string buf;
		for(auto &i:roundSession->sort){
			sprintf(commandBuffer,"/setblock %d %d %d iron_block",i->x,i->y,i->z);
			buf.assign(commandBuffer);
			mCmds->requestCommandExecution(origin.clone(),buf,12,true);
			//mCmds->requestCommandExecutionWhatever(buf,(Player*)origin.getEntity(),true);
			//break;
		}
		delete roundSession;
		output.success("Done");
		return;
	}
};*/


void (*GLPNCommand__setup)(CommandRegistry &cr);

void newGLPNCommandSetup(CommandRegistry &cr){
	//getposCommand::setup(cr);
	//buildRoundCommand::setup(cr);
	FBCommand::setup(cr);
	/*TestCommand::setup(cr);
	OpenSkinPickerCommand::setup(cr);
	SpawnExperienceOrbCommand::setup(cr);
	popDestroyStackCommand::setup(cr);
	KoChiExpCommand::setup(cr);
	ExpPoolBeginCommand::setup(cr);*/
	return GLPNCommand__setup(cr);
}

void (*SayCommand__setup)(CommandRegistry &cr);
void newSayCommandSetup(CommandRegistry &cr){
	//ServerTestCommand::setup(cr);
	return SayCommand__setup(cr);
}


CommandRegistry::Signature const *(*sub_10215B200)(const CommandRegistry *self,std::string const& cmd);

CommandRegistry::Signature const *CommandRegistry::findCommand(std::string const& cmd) const{
	return sub_10215B200(this,cmd);
}

void (*sub_10215A660)(CommandRegistry *self,CommandRegistry::Signature& a, CommandRegistry::Overload& b);

void CommandRegistry::registerOverloadInternal(CommandRegistry::Signature& a, CommandRegistry::Overload& b){
	return sub_10215A660(this,a,b);
}

void (*sub_102157E84)(CommandRegistry *self,std::string const&, char const*, CommandPermissionLevel, CommandFlag, CommandFlag);

void CommandRegistry::registerCommand(std::string const& a, char const* b, CommandPermissionLevel c, CommandFlag d, CommandFlag e){
	return sub_102157E84(this,a,b,c,d,e);
}

void (*sub_102144DEC)(CommandOutput *self);

void CommandOutput::success() {
	sub_102144DEC(this);
}

void (*sub_102144D04)(CommandOutput *self,std::string const & a,std::vector<CommandOutputParameter> const &params);

void CommandOutput::success(std::string const & a,std::vector<CommandOutputParameter> const &params){
	sub_102144D04(this,a,params);
}

void (*sub_102144864)(CommandOutput *self,std::string const & a,std::vector<CommandOutputParameter> const &params);

void CommandOutput::error(std::string const & a,std::vector<CommandOutputParameter> const &params){
	sub_102144864(this,a,params);
}

ClientInstance *(*sub_100095DB4)(MinecraftGame *self);
ClientInstance *MinecraftGame::getPrimaryClientInstance(){
	return sub_100095DB4(this);
}

MobileClient *(*sub_101006FBC)();
MobileClient *MobileClient::singleton(){
	return sub_101006FBC();
}

MinecraftGame *(*sub_101009BC8)(MobileClient* self);
MinecraftGame *MobileClient::getMinecraftGame(){
	return sub_101009BC8(this);
}

MinecraftCommands *(*sub_mcgetcommands)(Minecraft *);
MinecraftCommands *Minecraft::getCommands(){
	return sub_mcgetcommands(this);
}

void *(*mcc_exec)(const MinecraftCommands*,std::unique_ptr<CommandOrigin>,std::string const&,int,bool);
void *MinecraftCommands::requestCommandExecution(std::unique_ptr<CommandOrigin> origin,std::string const& cmd,int unk1,bool nooutput)const{
	return mcc_exec(this,std::move(origin),cmd,unk1,nooutput);
}//unk1=12

void *(*mcc_execcmd)(const MinecraftCommands*,std::string const&,Player*,int32_t);
void *MinecraftCommands::requestCommandExecutionWhatever(std::string const& cmd,Player *ori,bool isfromplayer)const{
	return mcc_execcmd(this,cmd,ori,isfromplayer);
}

void *(*origClientCommandsSetup)(MinecraftCommands *cmds,void *game,void *tg,void *fa,bool unk,const void *identity);
void *clientCommandsSetup(MinecraftCommands *cmds,void *game,void *tg,void *fa,bool unk,const void *identity){
	mCmds=cmds;
	return origClientCommandsSetup(cmds,game,tg,fa,unk,identity);
}

std::string (*CmdGetMessage)(const CommandMessage *,CommandOrigin const&);
std::string CommandMessage::getMessage(CommandOrigin const& origin) const{
	return CmdGetMessage(this,origin);
}

void (*performFunc_Intn)(MobileClient *,std::function<void()> const&);
void MobileClient::performFuncInMainThread(std::function<void()> const& func) {
	return performFunc_Intn(this,func);
}

GuiData *(*MCgetPriGuiData)(MinecraftGame *);
GuiData *MinecraftGame::getPrimaryGuiData(){
	return MCgetPriGuiData(this);
}

void (*GUIDATA_DisplaySysMsg)(GuiData *,std::string const &msg,std::string const& empty);
void GuiData::displaySystemMessage(std::string const &msg,std::string const& empty){
	return GUIDATA_DisplaySysMsg(this,msg,empty);
}

static NSString *getMCVersion(){
	return [[NSBundle mainBundle].infoDictionary[@"CFBundleVersion"]];
}

static void fbshownotcompatible(){
	__block UIWindow *alertWindow=[[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
	UIViewController *newController=[[UIViewController alloc] init];
	alertWindow.rootViewController=newController;
	alertWindow.windowLevel=UIWindowLevelAlert+1;
	[alertWindow makeKeyAndVisible];
	UIAlertController *warn=[UIAlertController alertControllerWithTitle:@"FastBuilder" message:@"FastBuilder isn't compatible with your version of Minecraft,please wait for update." preferredStyle:UIAlertControllerStyleAlert];
	[warn addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action){
		[warn dismissViewControllerAnimated:YES completion:^{
			alertWindow.hidden=YES;
			if([NSProcessInfo.processInfo isOperatingSystemAtLeastVersion:(NSOperatingSystemVersion){13,0,0}])[alertWindow setWindowScene:nil];
			alertWindow=nil;
		}];
	}]];
	[newController presentViewController:warn animated:YES completion:nil];
}

static void initcn(){
	if(![getMCVersion() isEqualToString:@"1.21.20.118837"]){
		return fbshownotcompatible();
	}
	fsa((void**)&sub_10215B200,/*0x10215B200*/0x10215B178);
	fsa((void**)&sub_10215A660,/*0x10215A660*/0x10215A5D8);
	fsa((void**)&sub_102157E84,/*0x102157E84*/0x102157DFC);
	fsa((void**)&sub_102144DEC,/*0x102144DEC*/0x102144D64);
	fsa((void**)&sub_102144D04,/*0x102144D04*/0x102144C7C);
	fsa((void**)&sub_102144864,/*0x102144864*/0x1021447F0);
	fsa((void**)&BEDROCK_Threading_ON_MAIN_THREAD,/*0x100E73470*/0x100E7343C);
	fsa((void**)&sub_101006FBC,/*101006FBC*/0x101006F80);
	fsa((void**)&sub_101009BC8,/*0x101009BC8*/0x101009B8C);
	fsa((void**)&sub_100095DB4,0x100095DB4);
	fsa((void**)&sub_mcgetcommands,0x1022C5168);
	fsa((void**)&mcc_execcmd,0x10217C9A4);
	fsa((void**)&mcc_exec,0x10217B3D8);
	fsa((void**)&cmdmsgtypeid,0x104F0EA18);
	fsa((void**)&cmdMsgParseFn,0x10214AD84);
	fsa((void**)&CmdGetMessage,0x10213CDAC);
	fsa((void**)&performFunc_Intn,0x10100B3CC);
	fsa((void**)&MCgetPriGuiData,0x1000AB7F4);
	fsa((void**)&GUIDATA_DisplaySysMsg,0x1001EE7A4);
	MSHookFunction((void*)(_dyld_get_image_vmaddr_slide(0)+/*0x10003CCEC*/0x10003DA2C),(void*)&newGLPNCommandSetup,(void**)&GLPNCommand__setup);
	MSHookFunction((void*)(_dyld_get_image_vmaddr_slide(0)+0x1021CF944),(void*)&newSayCommandSetup,(void**)&SayCommand__setup);
	MSHookFunction((void*)(_dyld_get_image_vmaddr_slide(0)+0x10003C1E0),(void*)&clientCommandsSetup,(void**)&origClientCommandsSetup);
}

static void initww(){
	if(![getMCVersion() isEqualToString:@"1.16.210.05"]){
		return fbshownotcompatible();
	}
	fsa((void**)&sub_10215B200,0x102A452FC);
	fsa((void**)&sub_10215A660,0x102A44744);
	fsa((void**)&sub_102157E84,0x102A4201C);
	fsa((void**)&sub_102144DEC,0x102A2DAF0);
	fsa((void**)&sub_102144D04,0x102A2D92C);
	fsa((void**)&sub_102144864,0x102A2D4A0);
	fsa((void**)&BEDROCK_Threading_ON_MAIN_THREAD,/*0x100E73470*/0x100E7343C);
	fsa((void**)&sub_101006FBC,/*101006FBC*/0x101006F80);
	fsa((void**)&sub_101009BC8,/*0x101009BC8*/0x101009B8C);
	fsa((void**)&sub_100095DB4,0x100095DB4);
	fsa((void**)&sub_mcgetcommands,0x1022C5168);
	fsa((void**)&mcc_execcmd,0x10217C9A4);
	fsa((void**)&mcc_exec,0x10217B3D8);
	fsa((void**)&cmdmsgtypeid,0x104F0EA18);
	fsa((void**)&cmdMsgParseFn,0x10214AD84);
	fsa((void**)&CmdGetMessage,0x10213CDAC);
	fsa((void**)&performFunc_Intn,0x10100B3CC);
	fsa((void**)&MCgetPriGuiData,0x1000AB7F4);
	fsa((void**)&GUIDATA_DisplaySysMsg,0x1001EE7A4);
	MSHookFunction((void*)(_dyld_get_image_vmaddr_slide(0)+/*0x10003CCEC*/0x10003DA2C),(void*)&newGLPNCommandSetup,(void**)&GLPNCommand__setup);
	MSHookFunction((void*)(_dyld_get_image_vmaddr_slide(0)+0x1021CF944),(void*)&newSayCommandSetup,(void**)&SayCommand__setup);
	MSHookFunction((void*)(_dyld_get_image_vmaddr_slide(0)+0x10003C1E0),(void*)&clientCommandsSetup,(void**)&origClientCommandsSetup);
}

static __attribute__((constructor(65535))) void fbinit(int __unused argc, char __unused **argv, char __unused **envp) {
	fsa((void**)&sub_10215B200,/*0x10215B200*/0x10215B178);
	fsa((void**)&sub_10215A660,/*0x10215A660*/0x10215A5D8);
	fsa((void**)&sub_102157E84,/*0x102157E84*/0x102157DFC);
	fsa((void**)&sub_102144DEC,/*0x102144DEC*/0x102144D64);
	fsa((void**)&sub_102144D04,/*0x102144D04*/0x102144C7C);
	fsa((void**)&sub_102144864,/*0x102144864*/0x1021447F0);
	fsa((void**)&BEDROCK_Threading_ON_MAIN_THREAD,/*0x100E73470*/0x100E7343C);
	fsa((void**)&sub_101006FBC,/*101006FBC*/0x101006F80);
	fsa((void**)&sub_101009BC8,/*0x101009BC8*/0x101009B8C);
	fsa((void**)&sub_100095DB4,0x100095DB4);
	fsa((void**)&sub_mcgetcommands,0x1022C5168);
	fsa((void**)&mcc_execcmd,0x10217C9A4);
	fsa((void**)&mcc_exec,0x10217B3D8);
	fsa((void**)&cmdmsgtypeid,0x104F0EA18);
	fsa((void**)&cmdMsgParseFn,0x10214AD84);
	fsa((void**)&CmdGetMessage,0x10213CDAC);
	fsa((void**)&performFunc_Intn,0x10100B3CC);
	fsa((void**)&MCgetPriGuiData,0x1000AB7F4);
	fsa((void**)&GUIDATA_DisplaySysMsg,0x1001EE7A4);
	MSHookFunction((void*)(_dyld_get_image_vmaddr_slide(0)+/*0x10003CCEC*/0x10003DA2C),(void*)&newGLPNCommandSetup,(void**)&GLPNCommand__setup);
	MSHookFunction((void*)(_dyld_get_image_vmaddr_slide(0)+0x1021CF944),(void*)&newSayCommandSetup,(void**)&SayCommand__setup);
	MSHookFunction((void*)(_dyld_get_image_vmaddr_slide(0)+0x10003C1E0),(void*)&clientCommandsSetup,(void**)&origClientCommandsSetup);
	globalFastBuilder=std::make_unique<FastBuilderiOS>();
	globalFastBuilder->internalCommandSender=[](std::string const& cmd){
		mCmds->requestCommandExecution(tempOrigin->clone(),cmd,12,true);
	};
	globalFastBuilder->internalPerformInMainThread=[](std::function<void()> const& func){
		MobileClient::singleton()->performFuncInMainThread(func);
	};
	globalFastBuilder->internalTextSender=[](std::string const& msg){
		MobileClient::singleton()->getMinecraftGame()->getPrimaryGuiData()->displaySystemMessage(msg,"");
	};
}
