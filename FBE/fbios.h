#pragma once
#include "../FB/main.h"
#include "objcpart.h"
#include "../FB/plugin.h"
#include <stdio.h>

#define FB_log(farg,...) \
{ \
	fprintf(_log_f_ptr,farg "\n",## __VA_ARGS__); \
	fflush(_log_f_ptr); \
}

class FastBuilderiOS : public FastBuilder {
protected:
	FILE *_log_f_ptr=nullptr;
public:
	std::function<void(std::string const&)> internalCommandSender;
	std::function<void(std::function<void()> const&)> internalPerformInMainThread;
	std::function<void(std::string const&)> internalTextSender;
	
	virtual void sendText(std::string const& msg) override;
	virtual void sendCommand(std::string const& cmd) override;
	virtual void internal_log(const char *content) override;
	virtual void performInMainThread(std::function<void()> const& func) override;
	bool loadPlugin(const char *path,int perm=0);
	FBPluginBridge makeBridge(int perm);
	FastBuilderiOS();
	virtual ~FastBuilderiOS();
};