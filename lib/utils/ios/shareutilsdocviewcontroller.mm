#include "utils/ios/shareutilsdocviewcontroller.h"

#import "utils/ios/shareutilsdocviewcontroller.h"

@interface IOSShareUtilsDocViewController ()
@end

@implementation IOSShareUtilsDocViewController
#pragma mark -
#pragma mark View Life Cycle
- (void)viewDidLoad
{
    [super viewDidLoad];
}
#pragma mark -
#pragma mark Document Interaction Controller Delegate Methods
- (UIViewController*)documentInteractionControllerViewControllerForPreview:
        (UIDocumentInteractionController*)controller
{
#pragma unused(controller)
    return self;
}
- (void)documentInteractionControllerDidEndPreview:(UIDocumentInteractionController*)controller
{
#pragma unused(controller)
    self.m_shareUtils->previewDone(self.m_url);
}
@end
