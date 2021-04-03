#include <mach-o/dyld.h>
void fastSetAddr(void** target,int64_t offset){
	*target=(void*)((int64_t)_dyld_get_image_vmaddr_slide(0)+offset);
}
