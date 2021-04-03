#include "../FB/main.h"
#include "colortable.h"
#include <png/png.h>
#include <cmath>

extern std::string getDocumentsDir();

class SABlock {
public:
	std::string name;
	unsigned char data;
	SABlock(const SABlock& bl){
		name=bl.name;
		data=bl.data;
	}
	SABlock()=default;
	SABlock(std::string n,unsigned char d){
		name=n;
		data=d;
	}
	bool isAir(){
		return (name=="air");
	}
	bool operator==(const SABlock& b) const{
		return (name==b.name&&data==b.data);
	}
	bool operator!=(const SABlock& b) const{
		return !(name==b.name&&data==b.data);
	}
	SABlock &operator=(const SABlock& b){
		name=b.name;
		data=b.data;
		return *this;
	}
};

class PColor {
public:
	unsigned char r,g,b;
	PColor()=default;
	PColor(unsigned char r1,unsigned char g1,unsigned char b1){
		r=r1,g=g1,b=b1;
	}
	~PColor()=default;
};

class FBEPaintMethod : public FBBuildMethod {
public:
	const char *commandName="paintimage";
	
	static float ColorDistance(const PColor &color1,const PColor &color2){
		int r1=color1.r,g1=color1.r,b1=color1.b;
		int r2=color2.r,g2=color2.r,b2=color2.b;
		int rmean=(r1+r2)>>1;
		int red=r1-r2;
		int green=g1-g2;
		int blue=b1-b2;
		float result=(((512+rmean)*red*red)>>8) + 4*green*green + (((767-rmean)*blue*blue)>>8);
		return sqrt(result);
	}
	
	int findMin(std::vector<float> &t){
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
	
	SABlock getBlock(const PColor &c){
		std::vector<float> list;
		const int size=colormap.size();
		unsigned char data;
		std::string bname;
		std::vector<unsigned char> color;
		for(unsigned short i=0;i<size;i++){
			std::tie(data,bname,color)=colormap[i];
			list.push_back(ColorDistance(c,PColor(color[0],color[1],color[2])));
		}
		int min=findMin(list);
		std::tie(data,bname,color)=colormap[min];
		return SABlock(bname,data);
	}
	
	virtual std::unique_ptr<FBBlocks> execute(ArgInput const& input) override {
		std::unique_ptr<FBBlocks> blocks=std::make_unique<FBBlocks>();
		FILE *img=fopen((getDocumentsDir()+(std::string)"/"+input.path).c_str(),"rb");
		if(!img){
			BSSendFormat("Failed to open file in provided path.");
			return nullptr;
		}
		{
			unsigned char signature[8];
			fread(signature,1,8,img);
			if(!png_check_sig(signature,8)){
				fclose(img);
				BSSendFormat("Invalid image file.");
				return nullptr;
			}
		}
		png_structp inputPic=png_create_read_struct(PNG_LIBPNG_VER_STRING,0,0,0);
		if(!inputPic){
			fclose(img);
			BSSendFormat("Internal error: Failed to create a read struct.");
			return nullptr;
		}
		png_infop inputInfo=png_create_info_struct(inputPic);
		if(!inputInfo){
			png_destroy_read_struct(&inputPic,NULL,NULL);
			fclose(img);
			BSSendFormat("Failed to create a png info struct.");
			return nullptr;
		}
		if(setjmp(png_jmpbuf(inputPic))){
			png_destroy_read_struct(&inputPic,&inputInfo,NULL);
			fclose(img);
			BSSendFormat("Unhandled png error");
			return nullptr;
		}
		png_init_io(inputPic,img);
		png_set_sig_bytes(inputPic,8);
		png_read_png(inputPic,inputInfo,PNG_TRANSFORM_STRIP_16|PNG_TRANSFORM_GRAY_TO_RGB|PNG_TRANSFORM_STRIP_ALPHA,0);
		
		png_uint_32 width=png_get_image_width(inputPic,inputInfo);
		png_uint_32 height=png_get_image_height(inputPic,inputInfo);
		png_byte channels=png_get_channels(inputPic,inputInfo);
		if(channels<3){
			png_destroy_read_struct(&inputPic,&inputInfo,0);
			fclose(img);
			BSSendFormat("Too few channels");
			return nullptr;
		}
		png_bytep *rowPixels=png_get_rows(inputPic,inputInfo);
		PColor *tempColor=new PColor;
		for(int y=0;y<height;y++){
			for(int x=0;x<width;x++){
				tempColor->r=rowPixels[y][3*x];
				tempColor->g=rowPixels[y][3*x+1];
				tempColor->b=rowPixels[y][3*x+2];
				SABlock kBlk=getBlock(*tempColor);
				blocks->push(input.x+x,input.y,input.z+y,kBlk.name,kBlk.data);
			}
		}
		delete tempColor;
		png_destroy_read_struct(&inputPic,&inputInfo,NULL);
		fclose(img);
		return blocks;
	}
};

void initFBEPaint(FastBuilder *fb){
	fb->addBuildMethod(std::make_unique<FBEPaintMethod>());
}