#pragma once
#include "argparser.h"

#define BSSendFormat(content,...) \
{ \
	char temp[512]; \
	sprintf(temp,content,## __VA_ARGS__); \
	input.fb->sendText((std::string)temp); \
}

struct FBVec3 {
	int x;
	int y;
	int z;
	std::shared_ptr<std::string> blockName;
	unsigned short data;
	FBVec3(int x,int y,int z):x(x),y(y),z(z) {}
	FBVec3(FBVec3 const& o) {
		*this=o;
	}
	FBVec3(int x,int y,int z,std::shared_ptr<std::string> blockName,unsigned short data=0):x(x),y(y),z(z),blockName(blockName),data(data) {}
	bool operator==(const FBVec3& b) const {
		return (x==b.x&&y==b.y&&z==b.z);
	}
	bool operator<(const FBVec3& b) const{
		std::vector<int> va={x,y,z};
		std::vector<int> vb={b.x,b.y,b.z};
		return (va<vb);
	}
	bool operator>(const FBVec3& b) const{
		std::vector<int> va={x,y,z};
		std::vector<int> vb={b.x,b.y,b.z};
		return (va>vb);
	}
	FBVec3 &operator=(const FBVec3& b){
		x=b.x;
		y=b.y;
		z=b.z;
		if(b.blockName){
			blockName=b.blockName;
			data=b.data;
		}
		return *this;
	}
};

//using FBBlocks=std::vector<FBVec3>;
struct FBBlocks {
	//const FBVec3 FBSmallestVec{INT_MAX,INT_MAX,INT_MAX};
	bool needMD=false;
	std::vector<FBVec3> sort;
	inline bool getDuplicate(int x,int y,int z){
		for(FBVec3 &item : sort){
			if(item.x==x&&item.y==y&&item.z==z){
				return true;
			}
		}
		return false;
	}
	void push(int x,int y,int z){
		if(needMD&&getDuplicate(x,y,z))return;
		sort.emplace_back(x,y,z);
	}
	void push(int x,int y,int z,std::string const& blockName,unsigned short data=0){
		if(needMD&&getDuplicate(x,y,z))return;
		sort.emplace_back(x,y,z,std::make_shared<std::string>(blockName),data);
	}
};

struct ArgInput;
class FBBuildMethod {
public:
	//const char *commandName="";
	
	virtual bool setup(){return true;}//DO NOT CALL IT BY YOURSELF,FB WILL CALL IT.
	virtual std::unique_ptr<FBBlocks> execute(ArgInput const& input)=0;
	FBBuildMethod()=default;
	virtual ~FBBuildMethod()=default;
	inline const char *getCommandName() {
		return *(const char**)((int64_t)this+sizeof(void*));
	}
};