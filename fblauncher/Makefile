TARGET := iphone:clang:latest:7.0
ARCHS := arm64 arm64e
INSTALL_TARGET_PROCESSES = FBLauncher

include $(THEOS)/makefiles/common.mk

APPLICATION_NAME = FBLauncher

MASONRYSOURCES = Masonry/NSLayoutConstraint+MASDebugAdditions.m Masonry/MASConstraintMaker.m Masonry/MASConstraint.m Masonry/NSArray+MASAdditions.m Masonry/MASLayoutConstraint.m Masonry/View+MASAdditions.m Masonry/MASViewAttribute.m Masonry/MASViewConstraint.m Masonry/ViewController+MASAdditions.m Masonry/MASCompositeConstraint.m
FBLauncher_FILES = main.m FBLAppDelegate.m FBLRootViewController.m $(MASONRYSOURCES)
FBLauncher_FRAMEWORKS = UIKit CoreGraphics
FBLauncher_CFLAGS = -fobjc-arc -Wno-missing-noescape
FBLauncher_LIBRARIES = objc

include $(THEOS_MAKE_PATH)/application.mk
