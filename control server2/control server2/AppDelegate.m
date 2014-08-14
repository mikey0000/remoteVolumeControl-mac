//
//  AppDelegate.m
//  control server2
//
//  Created by Michael Arthur on 8/08/14.
//  Copyright (c) 2014 Michael Arthur. All rights reserved.
//

#import "AppDelegate.h"


@interface AppDelegate ()
@property (weak) IBOutlet NSTextField *port;

@property (weak) IBOutlet NSWindow *window;
@property (strong) EchoServer *es;

@end

@implementation AppDelegate
            
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    
    
    
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

- (IBAction)startServer:(id)sender {
    self.es = [[EchoServer alloc] init];
    NSString *portString = [self.port stringValue];
    
    // Here, I use perform...afterDelay to put an action on the run-loop before
    // it starts running. That action will actually start the accept socket, and
    // AsyncSocket will then be able to create other activity on the run-loop.
    // But main() will have no other opportunity to do so; the run-loop does not
    // give me any way in, other than the AsyncSocket delegate methods.
    
    
    [self.es acceptOnPortString:portString];
//    [[NSRunLoop currentRunLoop] run];
}

- (IBAction)raiseVolume:(id)sender {
    AudioDeviceID theDevice = GetDefaultOutputDevice();
    
    Float32 currVol = GetVolumeScalar(theDevice, false, 1);
    NSLog(@"current volume %f",currVol);
    currVol+=(Float32).07;
    SetAppVolume(currVol, theDevice);
}


- (IBAction)lowerVolume:(id)sender {
    AudioDeviceID theDevice = GetDefaultOutputDevice();
    
    Float32 currVol = GetVolumeScalar(theDevice, false, 1);
    NSLog(@"current volume %f",currVol);
    currVol-=(Float32).07;
    SetAppVolume(currVol, theDevice);
    
    
}

void SetAppVolume(Float32 theVolume, AudioDeviceID theDevice)
{
//    result = AudioDeviceSetProperty(theDevice, NULL, 0, /* master channel */ true, kAudioDevicePropertyVolumeScalar, sizeof(Float32), &theVolume);
    
    UInt32 size = sizeof(Float32);
    
    AudioObjectPropertyAddress left = {
        kAudioDevicePropertyVolumeScalar,
        kAudioDevicePropertyScopeOutput,
        1 // use values 1 and 2 here, 0 (master) does not seem to work
    };
    AudioObjectPropertyAddress right = {
        kAudioDevicePropertyVolumeScalar,
        kAudioDevicePropertyScopeOutput,
        2 // use values 1 and 2 here, 0 (master) does not seem to work
    };
    
    OSStatus err;
    err = AudioObjectSetPropertyData(theDevice, &left, 0, NULL, size, &theVolume);
    err = AudioObjectSetPropertyData(theDevice, &right, 0, NULL, size, &theVolume);
}

//Float32 GetAppVolumeScalar(AudioDeviceID inDevice, bool inIsInput, UInt32 inChannel)
//{
//    Float32 theAnswer = 0;
//    UInt32 theSize = sizeof(Float32);
//    AudioObjectPropertyScope theScope = inIsInput ? kAudioDevicePropertyScopeInput : kAudioDevicePropertyScopeOutput;
//    AudioObjectPropertyAddress theAddress = { kAudioDevicePropertyVolumeScalar,
//        theScope,
//        inChannel };
//    
//    OSStatus theError = AudioObjectGetPropertyData(inDevice,
//                                                   &theAddress,
//                                                   0,
//                                                   NULL,
//                                                   &theSize,
//                                                   &theAnswer);
//    // handle errors
//    
//    return theAnswer;
//}



//AudioDeviceID GetDefaultOutputDevice()
//{
//    AudioDeviceID theAnswer = 0;
//    UInt32 theSize = sizeof(AudioDeviceID);
//    AudioObjectPropertyAddress theAddress = { kAudioHardwarePropertyDefaultOutputDevice,
//        kAudioObjectPropertyScopeGlobal,
//        kAudioObjectPropertyElementMaster };
//    
//    OSStatus theError = AudioObjectGetPropertyData(kAudioObjectSystemObject,
//                                                   &theAddress,
//                                                   0,
//                                                   NULL,
//                                                   &theSize,
//                                                   &theAnswer);
//    // handle errors
//    
//    return theAnswer;
//}


//void runServer()
//{
//    CFSocketRef myipv4cfsock = CFSocketCreate(
//                                              kCFAllocatorDefault,
//                                              PF_INET,
//                                              SOCK_STREAM,
//                                              IPPROTO_TCP,
//                                              kCFSocketAcceptCallBack, handleConnect, NULL);
//    CFSocketRef myipv6cfsock = CFSocketCreate(
//                                              kCFAllocatorDefault,
//                                              PF_INET6,
//                                              SOCK_STREAM,
//                                              IPPROTO_TCP,
//                                              kCFSocketAcceptCallBack, handleConnect, NULL);
//    
//    struct sockaddr_in sin;
//    
//    memset(&sin, 0, sizeof(sin));
//    sin.sin_len = sizeof(sin);
//    sin.sin_family = AF_INET; /* Address family */
//    sin.sin_port = htons(990); /* Or a specific port */
//    sin.sin_addr.s_addr= INADDR_ANY;
//    
//    CFDataRef sincfd = CFDataCreate(
//                                    kCFAllocatorDefault,
//                                    (UInt8 *)&sin,
//                                    sizeof(sin));
//    
//    CFSocketSetAddress(myipv4cfsock, sincfd);
//    CFRelease(sincfd);
//    
//    struct sockaddr_in6 sin6;
//    
//    memset(&sin6, 0, sizeof(sin6));
//    sin6.sin6_len = sizeof(sin6);
//    sin6.sin6_family = AF_INET6; /* Address family */
//    sin6.sin6_port = htons(990); /* Or a specific port */
//    sin6.sin6_addr = in6addr_any;
//    
//    CFDataRef sin6cfd = CFDataCreate(
//                                     kCFAllocatorDefault,
//                                     (UInt8 *)&sin6,
//                                     sizeof(sin6));
//    
//    CFSocketSetAddress(myipv6cfsock, sin6cfd);
//    CFRelease(sin6cfd);
//    
//    CFRunLoopSourceRef socketsource = CFSocketCreateRunLoopSource(
//                                                                  kCFAllocatorDefault,
//                                                                  myipv4cfsock,
//                                                                  0);
//    
//    CFRunLoopAddSource(
//                       CFRunLoopGetCurrent(),
//                       socketsource,
//                       kCFRunLoopDefaultMode);
//    
//    CFRunLoopSourceRef socketsource6 = CFSocketCreateRunLoopSource(
//                                                                   kCFAllocatorDefault,
//                                                                   myipv6cfsock,
//                                                                   0);
//    
//    CFRunLoopAddSource(
//                       CFRunLoopGetCurrent(),
//                       socketsource6,
//                       kCFRunLoopDefaultMode);
//    
//    
//}
//
//void handleConnect (
//                 CFSocketRef s,
//                 CFSocketCallBackType callbackType,
//                 CFDataRef address,
//                 const void *data,
//                 void *info
//                 )
//{
//    NSLog(@"data: %@", data);
//}

@end
