#include "fbios.h"
#include <dlfcn.h>

extern bool (*BEDROCK_Threading_ON_MAIN_THREAD)();

class FBPluginBridge;
FBPluginBridge FastBuilderiOS::makeBridge(int perm){
	FBPluginBridge bridge;
	bridge.perm=perm;
	bridge.addBuildMethod=[=](std::unique_ptr<FBBuildMethod> method)->bool{
		return addBuildMethod(std::move(method));
	};
	if(perm>=1){
		bridge.executeMCCommand=[=](std::string const& cmd){
			internalCommandSender(cmd);
		};
	}
	if(perm>=2){
		bridge.fastBuilder=this;
	}
	return bridge;
}

void FastBuilderiOS::sendCommand(std::string const& cmd){
	internalCommandSender(cmd);
}

bool FastBuilderiOS::loadPlugin(const char *path,int perm){
	void *bin=dlopen(path,RTLD_LAZY);
	if(!bin)return false;
	FBPlugin *(*initFunc)()=(FBPlugin *(*)())dlsym(bin,"FB__pluginInit__");
	FBPlugin *plugin=initFunc();
	if(!plugin){
		FB_log("loadPlugin: Fatal/ \"%s\": Init function haven't returned a plugin object.",path);
		dlclose(bin);
		return false;
	}
	if(!plugin->name){
		FB_log("loadPlugin: Fatal/ \"%s\": Name not set.",path);
		dlclose(bin);
		return false;
	}
	plugin->pluginBridge=std::make_unique<FBPluginBridge>(makeBridge(perm));
	plugin->init();
	FB_log("loadPlugin: Loaded plugin \"%s\".",plugin->name);
	return true;
}

void FastBuilderiOS::internal_log(const char *content){
	FB_log("%s",content);
}

FastBuilderiOS::FastBuilderiOS() {
	_log_f_ptr=fopen((getTempDir()+(std::string)"/fastbuilder.log").c_str(),"a");
	postChildInit();
}

FastBuilderiOS::~FastBuilderiOS() {
	fclose(_log_f_ptr);
}

void FastBuilderiOS::performInMainThread(std::function<void()> const& func) {
	return internalPerformInMainThread(func);
}

void FastBuilderiOS::sendText(std::string const& msg){
	if(BEDROCK_Threading_ON_MAIN_THREAD()){
		return internalTextSender(msg);
	}else{
		performInMainThread([=](){
			internalTextSender(msg);
		});
	}
}