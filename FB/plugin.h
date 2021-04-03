#pragma once
#include <string>
#include <stdint.h>
#include <functional>
#include <memory>

class FBPluginBridge {
public:
	int perm;
	FastBuilder *fastBuilder=nullptr;
	std::function<void(std::string const&)> executeMCCommand;
	std::function<bool(std::unique_ptr<FBBuildMethod>)> addBuildMethod;
};

class FBPlugin {
public:
	std::unique_ptr<FBPluginBridge> pluginBridge;
	const char *name=nullptr;
	const char *description=nullptr;
	
	//pluginBridge will be set before calling init().
	//Please set name and description before calling this function.
	virtual void init()=0;
};
//Symbol: __stdcall FBPlugin *FB__pluginInit__();