#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <memory>
#include "main.h"

class ArgParserGlobalSettings {
public:
	int x=0;
	int y=0;
	int z=0;
	std::shared_ptr<std::string> block;
	unsigned short data=0;
	ArgParserGlobalSettings(){
		block=std::make_shared<std::string>("iron_block");
	}
};

enum class FBShape {
	Hollow,
	Solid
};

enum class FBDirection : char {
	x='x',
	y='y',
	z='z'
};

class FastBuilder;
enum class FBCommandReturn;
struct ArgInput {
	std::vector<std::string> splited;
	std::shared_ptr<std::string> block;
	unsigned short data;
	FBDirection direction;
	FBShape shape;
	double radius;
	double width;
	double length;
	double height;
	double accuracy;
	std::string type;
	unsigned int tick;
	int x,y,z;
	std::string path;
	FastBuilder *fb;
	FBCommandReturn errorStatus;
};

class ArgParser {
private:
	void split(const std::string& s,std::vector<std::string>& sv,const char* delim = " ");
	ArgParserGlobalSettings globalObjects;
	//std::vector<std::string> validCommands;
public:
	/*std::vector<std::string> splited;
	std::shared_ptr<std::string> block;
	unsigned short data;
	char direction;
	std::shared_ptr<std::string> shape;
	double radius;
	double width;
	double length;
	double height;
	double accuracy;
	std::string type;
	unsigned int tick;
	int x,y,z;
	std::string path;*/

	~ArgParser()=default;

	ArgParser()=default;
	FBShape convertShape(std::string const &shape);
	const std::string convertShape(FBShape shape);
	bool checkDirection(char direction);
	ArgInput parseCommand(std::string const &cmd);
	void setPos(int x,int y,int z);
	//void addValidCommand(std::string const &command);
};
