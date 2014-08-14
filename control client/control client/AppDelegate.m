//
//  AppDelegate.m
//  control client
//
//  Created by Michael Arthur on 14/08/14.
//  Copyright (c) 2014 Michael Arthur. All rights reserved.
//
#import "AppDelegate.h"

@interface AppDelegate ()

@property (weak) IBOutlet NSTextField *address;
@property (weak) IBOutlet NSTextField *port;
@property (weak) IBOutlet NSButton *startClient;

@property (weak) IBOutlet NSWindow *window;

@property (strong) Echo *e;

@end



@implementation AppDelegate



- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
        
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}
- (IBAction)startClient:(id)sender {
    self.e = [[Echo alloc] init];
    [self.e connectToHost: [self.address stringValue] port:(UInt16)[self.port intValue]];

    
    
}

-(void) changeTextToConnected:(id) sender {
    self.startClient.title = @"connected";
}

- (IBAction)raiseVolumeServer:(id)sender {
    [self.e writeToServer:@"raise"];
}


- (IBAction)lowerVolumeServer:(id)sender {
    [self.e writeToServer:@"lower"];
    
}


@end


