#include "objcpart.h"
#include <Foundation/Foundation.h>

std::string getTempDir(){
	return std::string([[NSHomeDirectory() stringByAppendingPathComponent:@"tmp"] UTF8String]);
}

std::string getDocumentsDir(){
	return std::string([[NSHomeDirectory() stringByAppendingPathComponent:@"Documents"] UTF8String]);
}