#include "main.h"
#include "build.h"

#define PI 3.141592653589793

class _passableMethodHelperClass {
public:
	virtual std::unique_ptr<FBBuildMethod> newSelf()=0;
};

extern std::unique_ptr<FastBuilder> globalFastBuilder;
static std::vector<_passableMethodHelperClass *> _buildMethodList;

template <typename T>
class _buildMethodHelperClass : public _passableMethodHelperClass {
public:
	_buildMethodHelperClass(){
		_buildMethodList.push_back(this);
		if(globalFastBuilder){
			globalFastBuilder->addBuildMethod(newSelf());
		}
	}
	
	virtual std::unique_ptr<FBBuildMethod> newSelf() override{
		return std::make_unique<T>();
	}
};

#define _Internal_FBRegularMethod(methodName,processDuplication) \
class methodName##Method : public FBBuildMethod { \
public: \
	const char *commandName=#methodName; \
	void executeWithPos(int x,int y,int z,ArgInput const& input,FBBlocks &blocks); \
	virtual std::unique_ptr<FBBlocks> execute(ArgInput const& input) override { \
		std::unique_ptr<FBBlocks> blocks=std::make_unique<FBBlocks>(); \
		if(processDuplication){ \
			blocks->needMD=true; \
		} \
		executeWithPos(input.x,input.y,input.z,input,*blocks); \
		return blocks; \
	} \
}; \
static _buildMethodHelperClass<methodName##Method> __helper__##methodName##_method__STATIC_; \
void methodName##Method::executeWithPos(int x,int y,int z,ArgInput const& input,FBBlocks &blocks) 

#define FBRegularMethod(methodName) _Internal_FBRegularMethod(methodName,0)
#define FBRegularMethodProcessDuplication(methodName) _Internal_FBRegularMethod(methodName,1)


#define CreateFBBlocks(output) std::unique_ptr<FBBlocks> output=std::make_unique<FBBlocks>()
#define FBCreateBlocks CreateFBBlocks

FBRegularMethod(round) {
	//std::unique_ptr<FBBlocks> blocks=std::make_unique<FBBlocks>();
	FBDirection direction=input.direction;
	double radius=input.radius;
	switch (direction) {
		case FBDirection::x:
			for (double i = -radius; i <= radius; i++) {
				for (double j = -radius; j <= radius; j++) {
					if (i * i + j * j < radius * radius) {
						blocks.push(x,y+i,z+j);
					}
				}
			}
			break;
		case FBDirection::y:
			for (double i = -radius; i <= radius; i++) {
				for (double j = -radius; j <= radius; j++) {
					if (i * i + j * j < radius * radius) {
						blocks.push(x+i,y,z+j);
					}
				}
			}
			break;
		case FBDirection::z:
			for (double i = -radius; i <= radius; i++) {
				for (double j = -radius; j <= radius; j++) {
					if (i * i + j * j < radius * radius) {
						blocks.push(x+i,y + j,z);
					}
				}
			}
			break;
		default:
			break;
	}
	return;
}

FBRegularMethod(circle) {
	FBDirection direction=input.direction;
	double radius=input.radius;
	switch (direction) {
		case FBDirection::x:
			for (double i = -radius; i <= radius; i++) {
				for (double j = -radius; j <= radius; j++) {
					if (i * i + j * j < radius * radius && i * i + j * j >= (radius - 1) * (radius - 1)) {
						blocks.push(x,y+i,z+j);
					}
				}
			}
			break;
		case FBDirection::y:
			for (double i = -radius; i <= radius; i++) {
				for (double j = -radius; j <= radius; j++) {
					if (i * i + j * j < radius * radius && i * i + j * j >= (radius - 1) * (radius - 1)) {
						blocks.push(x+i,y,z+j);
					}
				}
			}
			break;
		case FBDirection::z:
			for (double i = -radius; i <= radius; i++) {
				for (double j = -radius; j <= radius; j++) {
					if (i * i + j * j < radius * radius && i * i + j * j >= (radius - 1) * (radius - 1)) {
						blocks.push(x+i,y+j,z);
					}
				}
			}
			break;
		default:
			break;
	}
	return;
}	

FBRegularMethod(sphere) {
	FBShape shape=input.shape;
	double radius=input.radius;
	if(shape==FBShape::Hollow){
		for (double i = -radius; i <= radius; i++) {
			for (double j = -radius; j <= radius; j++) {
				for (double k = -radius; k <= radius; k++) {
					if (i * i + j * j + k * k <= radius * radius && i * i + j * j + k * k >= (radius - 1) * (radius - 1)) {
						blocks.push(x+i,y+j,z+k);
					}
				}
			}
		}
	}else if(shape==FBShape::Solid){
		for (double i = -radius; i <= radius; i++) {
			for (double j = -radius; j <= radius; j++) {
				for (double k = -radius; k <= radius; k++) {
					if (i * i + j * j + k * k <= radius * radius) {
						blocks.push(x+i,y+j,z+k);
					}
				}
			}
		}
	}
	return;
}

FBRegularMethod(ellipse){
	FBDirection direction=input.direction;
	double length=input.length;
	double width=input.width;
	switch (direction) {
		case FBDirection::x:
			for (double i = -length; i <= length; i++) {
				for (double j = -width; j <= width; j++) {
					if ((i * i) / (length * length) + (j * j) / (width * width) < 1) {
						blocks.push(x,y+i,z+j);
					}
				}
			}
			break;
		case FBDirection::y:
			for (double i = -length; i <= length; i++) {
				for (double j = -width; j <= width; j++) {
					if ((i * i) / (length * length) + (j * j) / (width * width) < 1) {
						blocks.push(x+i,y,z+j);
					}
				}
			}
			break;
		case FBDirection::z:
			for (double i = -length; i <= length; i++) {
				for (double j = -width; j <= width; j++) {
					if ((i * i) / (length * length) + (j * j) / (width * width) < 1) {
						blocks.push(x+i,y+z,j);
					}
				}
			}
			break;
		default:
			break;
	}
	return;
}

FBRegularMethod(ellipsoid) {
	double length=input.length;
	double width=input.width;
	double height=input.height;
	for (double i = -length; i <= length; i++) {
		for (double j = -width; j <= width; j++) {
			for (double k = -height; k <= height; k++) {
				if ((i * i) / (length * length) + (j * j) / (width * width) + (k * k) / (height * height) <= 1) {
					blocks.push(x+1,y+j,z+k);
				}
			}
		}
	}
	return;
}

FBRegularMethodProcessDuplication(torus) {
	FBDirection direction=input.direction;
	double length=input.length;
	double radius=input.radius;
	double accuracy=input.accuracy;
	accuracy = 1 / accuracy;
	double max = PI * 2;
	switch (direction) {
		case FBDirection::x:
			for (double v = 0; v < max; v = v + accuracy) {
				for (double u = 0; u < max; u = u + accuracy) {
					blocks.push(round(cos(u) * (length * cos(v) + radius)) + x,round(sin(u) * (length * cos(v) + radius)) + y,round(length * sin(v)) + z);
				}
			}
			break;

		case FBDirection::y:
			for (double v = 0; v < max; v = v + accuracy) {
				for (double u = 0; u < max; u = u + accuracy) {
					blocks.push(round(cos(u) * (length * cos(v) + radius)) + x,round(length * sin(v)) + y,round(sin(u) * (length * cos(v) + radius)) + z);
				}
			}
			break;
		case FBDirection::z:
			for (double v = 0; v < max; v = v + accuracy) {
				for (double u = 0; u < max; u = u + accuracy) {
					blocks.push(round(length * sin(v)) + x,round(cos(u) * (length * cos(v) + radius)) + y,round(sin(u) * (length * cos(v) + radius)) + z);
				}
			}
			break;
		default:
			break;
	}
	return;
}

FBRegularMethodProcessDuplication(cone) {
	FBDirection direction=input.direction;
	double height=input.height;
	double radius=input.radius;
	double accuracy=input.accuracy;
	double max = PI * 2;
	accuracy = 1 / accuracy;
	switch (direction) {
		case FBDirection::z:
			for (double u = 0; u < height; u++) {
				for (double i = 0; i < max; i = i + accuracy) {
					blocks.push(floor(((height - u) / height) * radius * cos(i)) + x, floor(((height - u) / height) * radius * sin(i)) + y, u + z);
				}
			}
			break;
		case FBDirection::y:
			for (double u = 0; u < height; u++) {
				for (double i = 0; i < max; i = i + accuracy) {
					blocks.push(floor(((height - u) / height) * radius * cos(i)) + x, u + y, floor(((height - u) / height) * radius * sin(i)) + z);
				}
			}
			break;
		case FBDirection::x:
			for (double u = 0; u < height; u++) {
				for (double i = 0; i < max; i = i + accuracy) {
					blocks.push(u + x, floor(((height - u) / height) * radius * cos(i)) + y, floor(((height - u) / height) * radius * sin(i)) + z);
				}
			}
			break;
		default:
			break;
	}
	return;
}

FBRegularMethod(pyramid){
	double radius=input.radius;
	double height=input.height;
	for(double y = 0 ; y <= height ; y++){
		radius--;
		for (double x = 0 ; x <= radius ; x++){
			for(double z = 0 ; z <= radius ; z++){
				if((input.shape!=FBShape::Hollow && x <= radius && z <= radius) || (x == radius && y == radius)){
					blocks.push(x + x, y + y , z + z);
					blocks.push(x - x, y + y , z + z);
					blocks.push(x + x, y + y , z - z);
					blocks.push(x - x, y + y , z - z);
				}
			}
		}
	}
	return;
}

FBRegularMethodProcessDuplication(ellipticTorus){
	double radius=input.radius;
	double accuracy=input.accuracy;
	double length=input.length;
	double width=input.width;
	FBDirection direction=input.direction;
	accuracy = 1 / accuracy;
	double max = PI * 2;
	switch (direction) {
		case FBDirection::z:
			for (double v = 0; v < max; v = v + accuracy) {
				for (double u = 0; u < max; u = u + accuracy) {
					blocks.push(x + round((radius + (length * cos(v))) * cos(u)), y + round((radius + (length * cos(v))) * sin(u)), z + round(width * sin(v)));
				}
			}
			break;
		case FBDirection::y:
			for (double v = 0; v < max; v = v + accuracy) {
				for (double u = 0; u < max; u = u + accuracy) {
					blocks.push(x + round((radius + (length * cos(v))) * cos(u)), y + round(width * sin(v)), z + round((radius + (length * cos(v))) * sin(u)));
				}
			}
			break;
		case FBDirection::x:
			for (double v = 0; v < max; v = v + accuracy) {
				for (double u = 0; u < max; u = u + accuracy) {
					blocks.push(x + round(width * sin(v)), y + round((radius + (length * cos(v))) * sin(u)), z + round((radius + (length * cos(v))) * cos(u)));
				}
			}
			break;
		default:
			break;
	}
	return;
}

void initCoreMethods(FastBuilder *fb){
	for(auto *i:_buildMethodList){
		fb->addBuildMethod(i->newSelf());
	}
	/*
	fb->addBuildMethod(std::make_unique<roundMethod>());
	fb->addBuildMethod(std::make_unique<circleMethod>());
	fb->addBuildMethod(std::make_unique<sphereMethod>());
	fb->addBuildMethod(std::make_unique<ellipseMethod>());
	fb->addBuildMethod(std::make_unique<ellipsoidMethod>());
	fb->addBuildMethod(std::make_unique<torusMethod>());
	fb->addBuildMethod(std::make_unique<coneMethod>());
	fb->addBuildMethod(std::make_unique<pyramidMethod>());
	fb->addBuildMethod(std::make_unique<ellipticTorusMethod>());*/
}