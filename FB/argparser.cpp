#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <cstring>
#include <memory.h>
#include <iostream>
#include <map>
#include "main.h"

unsigned int str_f_replace(std::string& str,const std::string willreplace,const std::string to){
	unsigned int i=0;
	std::string::size_type n;
	while((n=str.find(willreplace))!=std::string::npos){
		str.replace(str.begin()+n,str.begin()+n+willreplace.size(),to);
		i++;
	}
	return i;
}

/*ArgParser::ArgParser(){
	validCommands={"stop","get","set","round","circle","sphere","ellipse","ellipsoid","torus","cone","pyramid","ellipticTorus"};
}*/

/*void ArgParser::addValidCommand(std::string const &command){
	return validCommands.push_back(command);
}*/

void ArgParser::split(const std::string& s,std::vector<std::string>& svn,const char* delim) {
	svn.clear();
	std::vector<std::string> sv;
	char* buffer = new char[s.size() + 1];
	memset((void*)buffer,0,s.size()+1);
	std::copy(s.begin(), s.end(), buffer);
	char* p = std::strtok(buffer, delim);
	do {
	sv.push_back(p);
	} while ((p = std::strtok(NULL, delim)));
	std::string lwa;
	bool lw=false;
	for(std::string str:sv){
		std::string rs=str;
		str_f_replace(rs,"\\\"","}}}}||||/||||{{{{");
		unsigned int iww=str_f_replace(rs,"\"","");
		str_f_replace(rs,"}}}}||||/||||{{{{","\"");
		if(0!=(iww%2)){
			if(!lw){
				lw=true;
				lwa=rs;
			}else{
				lw=false;
				lwa+=(std::string(" ")+rs);
				svn.push_back(lwa);std::cout<<lwa<<std::endl;
			}
		}else{
			if(!lw){
				svn.push_back(rs);
			}else{
				lwa+=(std::string(" ")+rs);
			}
		}
	}
	if(lw)throw std::string("Unterminated string.");
	return;
}

void ArgParser::setPos(int x,int y,int z){
	globalObjects.x=x;
	globalObjects.y=y;
	globalObjects.z=z;
}

FBShape ArgParser::convertShape(std::string const& shape){
	if(shape=="hollow"){
		return FBShape::Hollow;
	}else if(shape=="solid"){
		return FBShape::Solid;
	}else{
		throw std::runtime_error((std::string)"Invalid shape: "+shape);
	}
}

const std::string ArgParser::convertShape(FBShape shape){
	if(shape==FBShape::Hollow){
		return "hollow";
	}else if(shape==FBShape::Solid){
		return "solid";
	}else{
		throw std::runtime_error("convertShapeToString: Invalid shape");
	}
}

bool ArgParser::checkDirection(char direction){
	if(direction<'x'||direction>'z'){
		return false;
	}
	return true;
}

extern std::unique_ptr<FastBuilder> globalFastBuilder;

#define RETERROR(x) arg.errorStatus=x;return arg

ArgInput ArgParser::parseCommand(std::string const &cmd){
	ArgInput arg;
	arg.errorStatus=FBCommandReturn::OK;
	arg.x=globalObjects.x;
	arg.y=globalObjects.y;
	arg.z=globalObjects.z;
	arg.block=globalObjects.block;
	arg.data=globalObjects.data;
	arg.accuracy=50;
	arg.height=1;
	arg.length=2;
	arg.width=2;
	arg.shape=FBShape::Hollow;
	arg.direction=FBDirection::y;
	arg.tick=500;
	arg.fb=globalFastBuilder.get();

	if(cmd=="get"){
		arg.type="Mgetpos";
	}

	split(cmd,arg.splited);
	if(arg.splited.empty()){
		RETERROR(FBCommandReturn::InvalidInput);
		//throw std::runtime_error("Invalid input");
	}
	std::vector<std::string> &splited=arg.splited;
	arg.type=splited[0];//.substr(1);
	std::string &type=arg.type;
	/*bool valid=false;
	for(std::string const &i:validCommands){
		if(arg.type==i){
			valid=true;
			break;
		}
	}
	if(!valid){
		throw std::runtime_error("Invalid command");
	}*/
	for(int i=1;i<splited.size();i++){
		if(type=="get"){
			//if(splited[i]=="pos"){
				arg.type="Mgetpos";
				return arg;
			//}
			//throw std::runtime_error("Fell off from the face of the planet when resolving get command");
		}
		if(type=="let"){
			if(splited[i]=="block"&&i+1<splited.size()){
				type="Mdwrote";
				globalObjects.block=std::make_shared<std::string>(splited[i+1]);
				return arg;
			}else if(splited[i]=="data"&&i+1<splited.size()){
				type="Mdwrote";
				globalObjects.data=std::stoi(splited[i+1]);
				return arg;
			}else if(splited[i]=="pos"&&i+3<splited.size()){
				arg.type="Mdwrote";
				globalObjects.x=std::stoi(splited[i+1]);
				globalObjects.y=std::stoi(splited[i+2]);
				globalObjects.z=std::stoi(splited[i+3]);
				return arg;
			}else{
				throw std::runtime_error("invalid let");
			}
		}
		/*if(type=="get"){
			throw std::string("Unexpected GET format.");
		}else if(type=="let"){
			throw std::string("Unexpected LET format.");
		}*/
		if(i+1>=splited.size())break;
		if(splited[i]=="-f"){
			i++;
			arg.direction=(FBDirection)splited[i][0];
			if(!checkDirection((char)arg.direction)){
				//throw std::runtime_error("Invalid direction");
				RETERROR(FBCommandReturn::InvalidDirection);
			}
		}else if(splited[i]=="-s"){
			i++;
			try{
				arg.shape=convertShape(splited[i]);
			}catch(std::runtime_error &err){
				RETERROR(FBCommandReturn::InvalidShape);
			}
		}else if(splited[i]=="-z"){
			i++;
			arg.path=splited[i];
		}else if(splited[i]=="-p"){
			i++;
			arg.path=splited[i];
		}else if(splited[i]=="-t"){
			i++;
			arg.tick=std::stoi(splited[i]);
		}else if(splited[i]=="-w"){
			i++;
			arg.width=std::stod(splited[i]);
		}else if(splited[i]=="-l"){
			i++;
			arg.length=std::stod(splited[i]);
		}else if(splited[i]=="-h"){
			i++;
			arg.height=std::stoi(splited[i]);
		}else if(splited[i]=="-b"){
			i++;
			arg.block=std::make_shared<std::string>(splited[i]);
		}else if(splited[i]=="-bd"){
			i++;
			arg.data=std::stoi(splited[i]);
		}else if(splited[i]=="-d"){
			i++;
			arg.data=std::stoi(splited[i]);
		}else if(splited[i]=="-r"){
			i++;
			arg.radius=std::stod(splited[i]);
		}else if(splited[i]=="-a"){
			i++;
			arg.accuracy=std::stod(splited[i]);
		}
	}
	return arg;
}

#undef RETERROR
