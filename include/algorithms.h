#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <array>
#include <functional>
#include <climits>
//#include <minecraft/extra/json.h>
//#include "../libpng/png.h"
#include "colortable.h"
#include <fstream>
//#include <io/izlibstream.h>
//#include <io/stream_reader.h>
//#include <nbt_tags.h>
//#include "paletteresolver.h"

#define PI 3.141592653589793

class N_Vec3 {
public:
	int x,y,z;
	~N_Vec3(){}
	N_Vec3(int x1,int y1,int z1){
		x=x1;y=y1;z=z1;
	}
};

class N_Color {
public:
	unsigned char r,g,b;
	N_Color(unsigned char r1,unsigned char g1,unsigned char b1){
		r=r1,g=g1,b=b1;
	}
	~N_Color(){}
};

class N_Block {
public:
	char *name;
	unsigned char data;
	N_Block(std::string n,unsigned char d){
		const char *dump=n.c_str();
		name=(char*)malloc(n.size()+1);
		memset((void*)name,0,n.size()+1);
		memcpy((void*)name,(void*)dump,n.size());
		data=d;
	}
	~N_Block(){
		free(name);
	}
	bool isAir(){
		return (strcmp(name,"air")==0);
	}
};

class session {
public:
	std::vector<N_Vec3*> sort;
	bool needMD;
	~session(){
		for(N_Vec3 *i:sort){
			delete i;
		}
		sort.clear();
	}
	
	session(){
		needMD=false;
	}

	void push(int x,int y,int z){
		if(needMD){
			for(N_Vec3 *i:sort){
				if(i->x==x&&i->y==y&&i->z==z)return;
			}
		}
		int smallestc=0;
		N_Vec3 smallestVec(INT_MAX,INT_MAX,INT_MAX);
		int c=0;
		for(N_Vec3 *i:sort){
			if((i->x>x||i->y>y||i->z>z)&&(i->x<smallestVec.x&&i->y<smallestVec.y&&i->z<smallestVec.z)){
				smallestc=c;
				smallestVec.x=i->x;
				smallestVec.y=i->y;
				smallestVec.z=i->z;
			}
			c++;
		}
		sort.insert(sort.begin()+smallestc,new N_Vec3(x,y,z));
	}

};

class csession {
public:
	std::map<N_Vec3*,N_Block*> vmap;
	std::vector<N_Block*> deleted;
	std::vector<N_Vec3*> sort;
	bool lp;
	~csession(){
		for(auto i:vmap){
			delete i.first;
			bool break_out=false;
			for(N_Block *dblk:deleted){
				if(dblk==i.second){
					break_out=true;
					break;
				}
			}
			if(break_out)continue;
			delete i.second;
			deleted.push_back(i.second);
		}
		sort.clear();
		vmap.clear();
	}
	
	csession(){
		lp=false;
	}

	csession(bool lp_used){
		lp=lp_used;
	}

	void push(int x,int y,int z,std::string bn,unsigned char bd){
		N_Vec3 *tvec=new N_Vec3(x,y,z);
		vmap[tvec]=new N_Block(bn,bd);
		int smallestc=0;
		N_Vec3 smallestVec(INT_MAX,INT_MAX,INT_MAX);
		int c=0;
		for(N_Vec3 *i:sort){
			if((i->x>x||i->y>y||i->z>z)&&(i->x<smallestVec.x&&i->y<smallestVec.y&&i->z<smallestVec.z)){
				smallestc=c;
				smallestVec.x=i->x;
				smallestVec.y=i->y;
				smallestVec.z=i->z;
			}
			c++;
		}
		sort.insert(sort.begin()+smallestc,tvec);
	}

	void push(int x,int y,int z,N_Block *block){
		N_Vec3 *tvec=new N_Vec3(x,y,z);
		vmap[tvec]=block;
		int smallestc=0;
		N_Vec3 smallestVec(INT_MAX,INT_MAX,INT_MAX);
		int c=0;
		for(N_Vec3 *i:sort){
			if((i->x>x||i->y>y||i->z>z)&&(i->x<smallestVec.x&&i->y<smallestVec.y&&i->z<smallestVec.z)){
				smallestc=c;
				smallestVec.x=i->x;
				smallestVec.y=i->y;
				smallestVec.z=i->z;
			}
			c++;
		}
		sort.insert(sort.begin()+smallestc,tvec);
	}
};

class CoreBuildMethods {
public:
	static session *buildround(int x, int y, int z, char direction,double radius){
		session *sess = new session();
		switch (direction) {
			case 'x':
				for (double i = -radius; i <= radius; i++) {
					for (double j = -radius; j <= radius; j++) {
						if (i * i + j * j < radius * radius) {
							sess->push(x,y+i,z+j);
						}
					}
				}
				break;
			case 'y':
				for (double i = -radius; i <= radius; i++) {
					for (double j = -radius; j <= radius; j++) {
						if (i * i + j * j < radius * radius) {
							sess->push(x+i,y,z+j);
						}
					}
				}
				break;
			case 'z':
				for (double i = -radius; i <= radius; i++) {
					for (double j = -radius; j <= radius; j++) {
						if (i * i + j * j < radius * radius) {
							sess->push(x+i,y + j,z);
						}
					}
				}
				break;
			default:
				break;
		}
		return sess;
	}


	static session *buildcircle(int x, int y, int z, char direction,double radius){
		session *sess = new session();
		switch (direction) {
			case 'x':
				for (double i = -radius; i <= radius; i++) {
					for (double j = -radius; j <= radius; j++) {
						if (i * i + j * j < radius * radius && i * i + j * j >= (radius - 1) * (radius - 1)) {
							sess->push(x,y+i,z+j);
						}
					}
				}
				break;
			case 'y':
				for (double i = -radius; i <= radius; i++) {
					for (double j = -radius; j <= radius; j++) {
						if (i * i + j * j < radius * radius && i * i + j * j >= (radius - 1) * (radius - 1)) {
							sess->push(x+i,y,z+j);
						}
					}
				}
				break;
			case 'z':
				for (double i = -radius; i <= radius; i++) {
					for (double j = -radius; j <= radius; j++) {
						if (i * i + j * j < radius * radius && i * i + j * j >= (radius - 1) * (radius - 1)) {
							sess->push(x+i,y+j,z);
						}
					}
				}
				break;
			default:
				break;
		}
		return sess;
	}


	static session *buildsphere(int x,int y,int z,std::string shape,double radius){
		session *sess = new session();
		if(shape=="hollow"){
			for (double i = -radius; i <= radius; i++) {
				for (double j = -radius; j <= radius; j++) {
					for (double k = -radius; k <= radius; k++) {
						if (i * i + j * j + k * k <= radius * radius && i * i + j * j + k * k >= (radius - 1) * (radius - 1)) {
							sess->push(x+i,y+j,z+k);
						}
					}
				}
			}
		}else if(shape=="solid"){
			for (double i = -radius; i <= radius; i++) {
				for (double j = -radius; j <= radius; j++) {
					for (double k = -radius; k <= radius; k++) {
						if (i * i + j * j + k * k <= radius * radius) {
							sess->push(x+i,y+j,z+k);
						}
					}
				}
			}
		}
		return sess;
	}


	static session *buildellipse(int x,int y,int z,char direction,double length,double width){
		session *sess = new session();
		switch (direction) {
			case 'x':
				for (double i = -length; i <= length; i++) {
					for (double j = -width; j <= width; j++) {
						if ((i * i) / (length * length) + (j * j) / (width * width) < 1) {
							sess->push(x,y+i,z+j);
						}
					}
				}
				break;
			case 'y':
				for (double i = -length; i <= length; i++) {
					for (double j = -width; j <= width; j++) {
						if ((i * i) / (length * length) + (j * j) / (width * width) < 1) {
							sess->push(x+i,y,z+j);
						}
					}
				}
				break;
			case 'z':
				for (double i = -length; i <= length; i++) {
					for (double j = -width; j <= width; j++) {
						if ((i * i) / (length * length) + (j * j) / (width * width) < 1) {
							sess->push(x+i,y+z,j);
						}
					}
				}
				break;
			default:
				break;
		}
		return sess;
	}


	static session *buildellipsoid(int x,int y,int z,double length,double width,double height){
		session *sess = new session();
		for (double i = -length; i <= length; i++) {
			for (double j = -width; j <= width; j++) {
				for (double k = -height; k <= height; k++) {
					if ((i * i) / (length * length) + (j * j) / (width * width) + (k * k) / (height * height) <= 1) {
						sess->push(x+1,y+j,z+k);
					}
				}
			}
		}
		return sess;
	}


	static session *buildtorus(int x,int y,int z,char direction,double length,double radius,double accuracy){
		session *sess = new session();
		sess->needMD=true;
		accuracy = 1 / accuracy;
		double max = PI * 2;
		switch (direction) {
			case 'x':
				for (double v = 0; v < max; v = v + accuracy) {
					for (double u = 0; u < max; u = u + accuracy) {
						sess->push(round(cos(u) * (length * cos(v) + radius)) + x,round(sin(u) * (length * cos(v) + radius)) + y,round(length * sin(v)) + z);
					}
				}
				break;

			case 'y':
				for (double v = 0; v < max; v = v + accuracy) {
					for (double u = 0; u < max; u = u + accuracy) {
						sess->push(round(cos(u) * (length * cos(v) + radius)) + x,round(length * sin(v)) + y,round(sin(u) * (length * cos(v) + radius)) + z);
					}
				}
				break;
			case 'z':
				for (double v = 0; v < max; v = v + accuracy) {
					for (double u = 0; u < max; u = u + accuracy) {
						sess->push(round(length * sin(v)) + x,round(cos(u) * (length * cos(v) + radius)) + y,round(sin(u) * (length * cos(v) + radius)) + z);
					}
				}
				break;
			default:
				break;
		}
		return sess;//multiDimensionalUnique(session);
	}

	static session *buildcone(int x,int y,int z,char direction,double height,double radius,double accuracy){
		session *sessio = new session();
		sessio->needMD=true;
		double max = PI * 2;
		accuracy = 1 / accuracy;
		switch (direction) {
			case 'z':
				for (double u = 0; u < height; u++) {
					for (double i = 0; i < max; i = i + accuracy) {
						sessio->push(floor(((height - u) / height) * radius * cos(i)) + x, floor(((height - u) / height) * radius * sin(i)) + y, u + z);
					}
				}
				break;
			case 'y':
				for (double u = 0; u < height; u++) {
					for (double i = 0; i < max; i = i + accuracy) {
						sessio->push(floor(((height - u) / height) * radius * cos(i)) + x, u + y, floor(((height - u) / height) * radius * sin(i)) + z);
					}
				}
				break;
			case 'x':
				for (double u = 0; u < height; u++) {
					for (double i = 0; i < max; i = i + accuracy) {
						sessio->push(u + x, floor(((height - u) / height) * radius * cos(i)) + y, floor(((height - u) / height) * radius * sin(i)) + z);
					}
				}
				break;
			default:
				break;
		}

		return sessio;//multiDimensionalUnique(session);
	}

	static session *buildpyramid(int x,int y,int z,std::string shape,double radius,double height){
		session *sess = new session();
		for(double y = 0 ; y <= height ; y++){
			radius--;
			for (double x = 0 ; x <= radius ; x++){
				for(double z = 0 ; z <= radius ; z++){
					if((shape!="hollow" && x <= radius && z <= radius) || (x == radius && y == radius)){
						sess->push(x + x, y + y , z + z);
						sess->push(x - x, y + y , z + z);
						sess->push(x + x, y + y , z - z);
						sess->push(x - x, y + y , z - z);
					}
				}
			}
		}
		return sess;
	}

	static session *buildellipticTorus(int x,int y,int z,double radius,double accuracy,double length,double width,char direction){
		session *sess=new session();
		sess->needMD=true;
		accuracy = 1 / accuracy;
		double max = PI * 2;
		switch (direction) {
			case 'z':
				for (double v = 0; v < max; v = v + accuracy) {
					for (double u = 0; u < max; u = u + accuracy) {
						sess->push(x + round((radius + (length * cos(v))) * cos(u)), y + round((radius + (length * cos(v))) * sin(u)), z + round(width * sin(v)));
					}
				}
				break;
			case 'y':
				for (double v = 0; v < max; v = v + accuracy) {
					for (double u = 0; u < max; u = u + accuracy) {
						sess->push(x + round((radius + (length * cos(v))) * cos(u)), y + round(width * sin(v)), z + round((radius + (length * cos(v))) * sin(u)));
					}
				}
				break;
			case 'x':
				for (double v = 0; v < max; v = v + accuracy) {
					for (double u = 0; u < max; u = u + accuracy) {
						sess->push(x + round(width * sin(v)), y + round((radius + (length * cos(v))) * sin(u)), z + round((radius + (length * cos(v))) * cos(u)));
					}
				}
				break;
			default:
				break;
		}
		return sess;//multiDimensionalUnique(session);
	}
#if 0
	static N_Block *getN_Block(N_Color c){
		std::vector<float> list;
		const int size=colormap.size();
		unsigned char data;
		std::string bname;
		std::vector<unsigned char> color;
		for(unsigned short i=0;i<size;i++){
			std::tie(data,bname,color)=colormap[i];
			list.push_back(N_ColorDistance(c,N_Color(color[0],color[1],color[2])));
		}
		int min=findMin(list);
		std::tie(data,bname,color)=colormap[min];
		return new N_Block(bname,data);
	}

	static csession *draw(std::vector<N_Block*> list,unsigned int w,unsigned int h,int xx,int yy,int zz){
		int x=xx,y=yy,z=zz+h-1;
		csession *rsl=new csession();
		int max = w + x;
		int min = x;
		int t = 0;
		while(1){
			if(x == max){
				z = z - 1;
				x = min;
			}

			rsl->push(x,y,z,list[t]->name,list[t]->data);
			x+=1;

			t++;
			if(t == list.size()){
				break;
			}
		}
		return rsl;
	}

	static float N_ColorDistance(N_Color color1,N_Color color2){
		int r1=color1.r,g1=color1.r,b1=color1.b;
		int r2=color2.r,g2=color2.r,b2=color2.b;
		int rmean=(r1+r2)>>1;
		int red=r1-r2;
		int green=g1-g2;
		int blue=b1-b2;
		float result=(((512+rmean)*red*red)>>8) + 4*green*green + (((767-rmean)*blue*blue)>>8);
		return sqrt(result);
	}

	static int findMin(std::vector<float> t){
		int min=t[0];
		int index=0;
		for(int n=1;n<t.size();n++){
			if(t[n]<min){
				min=t[n];
				index=n;
			}
		}
		return index;
	}

	static csession *Paint(int x, int y, int z,std::string path,CommandOutput& output){
		FILE *img=fopen(path.c_str(),"r");
		if(!img){
			output.error("Non-Exist file.");
			return nullptr;
		}
		unsigned char sig[8];
		fread(sig,1,8,img);
		if(!png_check_sig(sig,8)){
			output.error("Not a png file.");
			fclose(img);
			return nullptr;
		}
		png_structp png=png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
		png_infop info=png_create_info_struct(png);
		png_init_io(png,img);
		png_set_sig_bytes(png,8);
		png_read_info(png,info);

		png_uint_32 width = png_get_image_width(png, info);
		png_uint_32 height = png_get_image_height(png, info);
		png_uint_32 bitDepth = png_get_bit_depth(png, info);
		png_uint_32 channels = png_get_channels(png, info);
		if(channels<3){
			output.error("Channels must >= 3.");
			fclose(img);
			return nullptr;
		}

		png_bytep* rowPtrs=new png_bytep[height];
		unsigned long dataSize = width * height * bitDepth * channels / 8;
		unsigned char* data = new unsigned char[dataSize];
		const unsigned int stride = width * bitDepth * channels / 8;
		for (size_t i = 0; i < height; ++i) {
			png_uint_32 q = (height - i - 1) * stride;
			rowPtrs[i] = (png_bytep)data + q;
		}
		png_read_image(png, rowPtrs);

		std::vector<unsigned char> _d={0,0,0};
		int _dlast=0;
		std::vector<N_Block*> BuildList;

		for (int i = 0 ; i < width*height*channels; i++){
			_d[_dlast]=data[i];
			_dlast++;
			if(_dlast==channels/*i != 0 && (i + 1) % 4 == 0*/){
				BuildList.push_back(getN_Block(N_Color(_d[0],_d[1],_d[2])));
				_dlast=0;
			}
		}
		
		csession *ss=draw(BuildList, width, height, x,y,z);
		delete[] rowPtrs;
		delete[] data;
		fclose(img);
		return ss;
	}


	static csession *buildNBT(int x, int y, int z,std::string path,CommandOutput& output){
		try{
		std::ifstream file(path,std::ios::binary);
		zlib::izlibstream nbtz(file);
		auto rootpair=nbt::io::read_compound(nbtz);
		nbt::tag_list palette=rootpair.second->at("palette").as<nbt::tag_list>();
		std::map<unsigned int,N_Block*> resolvedPalettes;
		unsigned int _end=0;
		for(nbt::value &item:palette){
			std::string name=static_cast<const std::string&>(item.at("Name"));
			unsigned char data=0;
			name.replace(name.begin(),name.begin()+10,"");
			std::string::size_type st=name.find("door");
			std::string::size_type st_=name.find("trapdoor");
			if(st!=std::string::npos&&st_==std::string::npos){
				if(static_cast<const std::string&>(item.at("Properties").at("half"))=="upper"){
					resolvedPalettes[_end]=nullptr;
					_end++;
					continue;
				}
			}
			Json::Reader reader;
			Json::Value root;
			reader.parse(palette_json,root);
			if(root["minecraft"][name].isArray()){
				int size=root["minecraft"][name].size();
				bool ok=true;
				for(int i=0;i<size;i++){
					ok=true;
					std::vector<std::string> memb=root["minecraft"][name][i]["block_state"].getMemberNames();
					for(auto it=memb.begin();it!=memb.end();it++){
						if(*it!="shape"&&item.at("Properties").as<nbt::tag_compound>().has_key(*it)&&(static_cast<const std::string&>(item.at("Properties").at(*it))!=root["minecraft"][name][i]["block_state"][*it].asString(""))){
							//std::cout<<name<<std::endl;
							//std::cout<<static_cast<const std::string&>(item.at("Properties").at(*it))<<"||"<<root["minecraft"][name][i]["block_state"][*it].asString()<<std::endl;
							ok=false;
							break;
						}
					}
					if(ok){
						data=root["minecraft"][name][i]["data_value"]["meta"].asInt(0);
						break;
					}
				}
			}
			if(name=="sea_lantern")name="sealantern";
			resolvedPalettes[_end]=new N_Block(name,data);
			_end++;
		}
		nbt::tag_list blocks=rootpair.second->at("blocks").as<nbt::tag_list>();
		csession *ssr=new csession(true);
		for(nbt::value &item:blocks){
			N_Block *block=resolvedPalettes[static_cast<int64_t>(item.at("state"))];
			if(block==nullptr)continue;
			if(block->isAir())continue;
			nbt::tag_list pos=item.at("pos").as<nbt::tag_list>();
			ssr->push(x+static_cast<int64_t>(pos[0]),y+static_cast<int64_t>(pos[1]),z+static_cast<int64_t>(pos[2]),block);
		}
		return ssr;
		}catch(std::exception e){
			output.error(std::string("NBT: Failed.\nwhat(): ")+std::string(e.what()));
			return nullptr;
		}
	}
#endif
};
