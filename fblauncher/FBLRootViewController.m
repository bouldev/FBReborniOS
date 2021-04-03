#import "FBLRootViewController.h"
#import "Masonry/Masonry.h"

@interface FBLRootViewController ()

@end

@implementation FBLRootViewController

- (instancetype)init {
	self=[super init];
	self.view.backgroundColor=[UIColor whiteColor];
	self.navigationItem.title=@"FastBuilder R";
	self.navigationItem.rightBarButtonItems=@[[[UIBarButtonItem alloc] initWithTitle:@"Settings" style:UIBarButtonItemStylePlain target:self action:@selector(settingsClicked)]];
	UIButton *launchButton=[UIButton buttonWithType:UIButtonTypeSystem];
	launchButton.backgroundColor=[UIColor colorWithRed:0.259 green:0.518 blue:1 alpha:1];
	launchButton.layer.cornerRadius=24.5;
	[launchButton setTitle:@"启动" forState:UIControlStateNormal];
	[launchButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
	//[launchButton addTarget:self selector:@selector(launchClick) forControlEvents:UIControlEventTouchUpInside];
	[self.view addSubview:launchButton];
	[launchButton mas_makeConstraints:^(MASConstraintMaker *make){
		make.centerX.mas_equalTo(self.view.mas_centerX);
		make.bottom.mas_equalTo(self.view.mas_bottom).offset(-140);
		make.size.mas_equalTo(CGSizeMake(197,49));
	}];
	_launchButton=launchButton;
	return self;
}

- (void)settingsClicked {
}

- (void)loadView {
	[super loadView];
}

- (void)viewDidLoad {
	[super viewDidLoad];
}

- (void)launchClick {
	
}

@end
