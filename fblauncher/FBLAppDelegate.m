#import "FBLAppDelegate.h"
#import "FBLRootViewController.h"

@implementation FBLAppDelegate

- (void)applicationDidFinishLaunching:(UIApplication *)application {
	_window = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
	UINavigationController *rootNavController=[[UINavigationController alloc] initWithRootViewController:[[FBLRootViewController alloc] init]];
	UITabBarController *tabVC=[[UITabBarController alloc] init];
	[tabVC setViewControllers:@[rootNavController] animated:NO];
	for(int i=0;i<tabVC.tabBar.items.count;i++){
		if(i==0){
			tabVC.tabBar.items[i].title=@"FastBuilder";
			tabVC.tabBar.items[i].image=[UIImage systemImageNamed:@"star"];
		}
	}
	//tabVC.tabBar.items=[[UITabBarItem alloc] initWithTitle:@"FB" image:[UIImage systemImageNamed:@"xserve"] tag:0];
	_rootViewController = tabVC;
	_window.rootViewController = _rootViewController;
	[_window makeKeyAndVisible];
}

@end
