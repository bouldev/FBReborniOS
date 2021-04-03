TARGET := iphone:clang:11.2:9.0
ARCHS := arm64
INSTALL_TARGET_PROCESSES = minecraftpe


include $(THEOS)/makefiles/common.mk

TWEAK_NAME = FBReborn

FBReborn_FILES = Tweak.xm fastsetaddr.cpp FB/argparser.cpp FB/main.cpp FBE/fbios.cpp FBE/objcpart.mm FB/coremethods.cpp extra/extra.cpp extra/paint.cpp
FBReborn_CFLAGS = -fobjc-arc -std=c++17 -Wno-unneeded-internal-declaration -Wno-c++17-extensions -Iinclude
FBReborn_LIBRARIES = c++abi z
FBReborn_OBJ_FILES = lib/libpng.a
FBReborn_LDFLAGS = 

include $(THEOS_MAKE_PATH)/tweak.mk
SUBPROJECTS += fblauncher
include $(THEOS_MAKE_PATH)/aggregate.mk
