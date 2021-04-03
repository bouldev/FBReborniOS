#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <unistd.h>
#include <thread>
#include <ctime>

#include "main.h"

extern void initCoreMethods(FastBuilder *fb);
extern void initFBExtra(FastBuilder *fb);

bool FastBuilder::isGetPosCommand(std::string const& cmd){
	if(cmd=="get")return true;
	if(cmd.substr(0,7)=="get pos")return true;
	return false;
}

void FastBuilder::sendText(std::string const& message){
	return sendCommand((std::string)"/tell @s §\"[FB]>> " + message + "§\"");
}

FBCommandReturn FastBuilder::executeFBCommand(std::string const& cmd){
	ArgInput input=argParser.parseCommand(cmd);
	if(input.errorStatus!=FBCommandReturn::OK)return input.errorStatus;
	if(input.type=="Mgetpos")return FBCommandReturn::IsGetPos;
	if(input.type=="Mdwrote")return FBCommandReturn::OK;
	auto methodsc=methods.find(input.type);
	if(methodsc==methods.end()){
		return FBCommandReturn::NotFoundError;
	}
	std::thread([=](){
		sendText("Async/Build: Calculating...");
		clock_t start_calc_time=clock();
		auto &methodptr=methodsc->second;
		//FBBuildMethod &method=*methodptr;
		std::unique_ptr<FBBlocks> resultptr=methodptr->execute(input);
		if(!resultptr||resultptr->sort.size()==0){
			sendText("Async/Build: Build method didn't return an output.");
			return;
		}
		std::vector<FBVec3> &result=resultptr->sort;
		double calctime=(double)(clock()-start_calc_time)/CLOCKS_PER_SEC;
		FBSendFormat("Async/Build: Calculation duration: %fs.",calctime);
		FBSendFormat("Async/Build: Building structure,total %lu blocks.",result.size());
		FBBlocks *resultrel=resultptr.release();
		performInMainThread([this,calctime,resultptr=resultrel,inputtype=input.type,inputblock=input.block,inputdata=input.data](){
			clock_t start_build_time=clock();
			std::vector<FBVec3> &result=resultptr->sort;
			bool isCustom=(bool)result[0].blockName;
			//std::vector<FBVec3> &result=resultptr->sort;
			char commandBuffer[1024];
			std::shared_ptr<std::string> blockName=inputblock;
			unsigned short blockData=inputdata;
			std::string command;
			//int atocommands=1000;
			//unsigned int counter=2;
			for(FBVec3 &info:result){
				if(isCustom){
					if(info.blockName->length()>960){
						FBIntnLog("Error executing command %s: The block name of position %d,%d,%d is too long and may causes stack overflow.",inputtype.c_str(),info.x,info.y,info.z);
						sendText("Async/Build: Internal error,please view log.");
						return;
					}
					blockName=info.blockName;
					blockData=info.data;
				}
				sprintf(commandBuffer,"/setblock %d %d %d %s %d",info.x,info.y,info.z,blockName->c_str(),(int)blockData);
				command.assign(commandBuffer);
				sendCommand(command);
				/*atocommands--;
				if(counter==0){
					sendText("Async/Build: 10000 commands sent,let's wait 10s.");
					sleep(10);
					counter=5;
				}
				if(atocommands==0){
					counter--;
					//sendText("Async/Build: Let's wait for 5s...");
					//sleep(5);
					sendText("Async/Build: Entering slow period...");
				}else if(atocommands<0){
					usleep(1500);
					//sendText("Async/Build: Let's wait for 10s...");
					if(atocommands<=-1000){
						counter--;
						if(counter<=0){
							if(counter>=-5){
								atocommands=-1;
								continue;
							}else{
								counter=2;
							}
						}
						//sendText("Async/Build: Let's wait for 2s...");
						//sleep(2);
						sendText("Async/Build: Slow period ended,entering fast period...");
						atocommands=1000;
					}
				}*/
			}
			double buildtime=(double)(clock()-start_build_time)/CLOCKS_PER_SEC;
			sendText("Async/Build: Structure built!");
			FBSendFormat("== SUMMARY ==\nCalculation duration: %fs\nBuilding duration:%fs\nTotal blocks:%lu\nAverage speed: %f blocks/s\n=============",calctime,buildtime,result.size(),result.size()/buildtime);
			delete resultptr;
		});
	}).detach();
	return FBCommandReturn::OK;
}

bool FastBuilder::addBuildMethod(std::unique_ptr<FBBuildMethod> method){
	const char *commandName=method->getCommandName();
	FBIntnLog("Registered method: %s.",commandName);
	int strlength=strlen(commandName);
	if((strlength>=32&&strlength<1)||strstr(commandName," ")!=NULL){
		FBIntnLog("addBuildMethod/Error: Found a method with invalid name,won't set it up.");
		return false;
	}else if(strlength>=10){
		FBIntnLog("addBuildMethod/Warning: The name of method \"%s\" is a bit long,it's unrecommended to give a long name because it may makes user hard to remember or type it.",commandName);
	}
	if(!method->setup()){
		FBIntnLog("addBuildMethod: method %s's setup() returned false,aborting.",commandName);
		return false;
	}
	methods[std::string(commandName)]=std::move(method);
	return true;
}

void FastBuilder::postChildInit() {
	initCoreMethods(this);
	initFBExtra(this);
}

void FastBuilder::setPos(int x,int y,int z) {
	argParser.setPos(x,y,z);
}

const char *FastBuilder::GetCommandReturn(FBCommandReturn value){
	switch(value){
	case FBCommandReturn::OK:
		return "Command executed successfully.";
	case FBCommandReturn::InvalidInput:
		return "Invalid input.";
	case FBCommandReturn::InvalidDirection:
		return "Invalid direction.";
	case FBCommandReturn::InvalidShape:
		return "Invalid shape.";
	case FBCommandReturn::IsGetPos:
		return "Get pos not handled!!";
	case FBCommandReturn::NotFoundError:
		return "Command not found.";
	case FBCommandReturn::NoBuildOutput:
		return "Build method didn't return an output.";
	case FBCommandReturn::InternalError:
		return "Internal error,please view log.";
	default:
		return "UNHANDLED_RETURN";
	}
	return "???";
}