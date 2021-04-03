#pragma once
#include <string>
#include <functional>
#include <unordered_map>
#include "build.h"
#include "argparser.h"

#define FBIntnLog(content,...) \
{ \
	sprintf(logBuffer,content,## __VA_ARGS__); \
	internal_log(logBuffer); \
}

#define FBSendFormat(content,...) \
{ \
	sprintf(logBuffer,content,## __VA_ARGS__); \
	sendText((std::string)logBuffer); \
}

enum class FBCommandReturn {
	OK,
	InvalidInput,
	InvalidDirection,
	InvalidShape,
	IsGetPos,
	NotFoundError,
	NoBuildOutput,
	InternalError
};

class ArgParser;
class FastBuilder {
protected:
	ArgParser argParser;
	char logBuffer[512];
	std::unordered_map<std::string,std::unique_ptr<FBBuildMethod>> methods;
public:
	FastBuilder()=default;
	void postChildInit();
	virtual void sendCommand(std::string const& cmd)=0;
	virtual void internal_log(const char *content)=0;
	virtual void performInMainThread(std::function<void()> const& func)=0;
	virtual void sendText(std::string const& message);
	//Should catch std::runtime_error out of this func.
	FBCommandReturn executeFBCommand(std::string const& cmd);
	bool isGetPosCommand(std::string const& cmd);
	bool addBuildMethod(std::unique_ptr<FBBuildMethod> method);
	void setPos(int x,int y,int z);
	static const char *GetCommandReturn(FBCommandReturn value);
};
