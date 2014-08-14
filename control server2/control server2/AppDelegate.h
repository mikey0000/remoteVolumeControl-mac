//
//  AppDelegate.h
//  control server2
//
//  Created by Michael Arthur on 8/08/14.
//  Copyright (c) 2014 Michael Arthur. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <CoreAudio/CoreAudio.h>
#include <CoreFoundation/CoreFoundation.h>
#include <sys/socket.h>
#include <netinet/in.h>
#import "AsyncSocket.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>


@end


@interface EchoServer : NSObject <GCDAsyncSocketDelegate>
{
    NSMutableArray *sockets;
    GCDAsyncSocket *listenSocket;
}
-(id) init;
-(void) dealloc;
-(void) acceptOnPortString:(NSString *)str;
//-(void) onSocket:(GCDAsyncSocket *)sock willDisconnectWithError:(NSError *)err;
//-(void) onSocket:(GCDAsyncSocket *)sock didAcceptNewSocket:(GCDAsyncSocket *)newSocket;
-(void) onSocket:(GCDAsyncSocket *)sock didConnectToHost:(NSString *)host port:(UInt16)port;
//-(void) onSocket:(GCDAsyncSocket *)sock didReadData:(NSData*)data withTag:(long)tag;
//-(void) onSocket:(GCDAsyncSocket *)sock didWriteDataWithTag:(long)tag;
@end

@implementation EchoServer


/*
 This method sets up the accept socket, but does not actually start it.
 Once started, the accept socket accepts incoming connections and creates new
 instances of AsyncSocket to handle them.
 Echo Server keeps the accept socket in index 0 of the sockets array and adds
 incoming connections at indices 1 and up.
 */
-(id) init
{
    self = [super init];
    sockets = [[NSMutableArray alloc] initWithCapacity:2];
    
    listenSocket = [[GCDAsyncSocket alloc] initWithDelegate:self delegateQueue:dispatch_get_main_queue()];
    [sockets addObject:listenSocket];
    return self;
}


/*
 This method will never get called, because you'll be using Ctrl-C to exit the
 app.
 */
-(void) dealloc
{
    // Releasing a socket will close it if it is connected or listening.
}


- (void)raiseVolume {
    AudioDeviceID theDevice = GetDefaultOutputDevice();
    
    Float32 currVol = GetVolumeScalar(theDevice, false, 1);
    NSLog(@"current volume %f",currVol);
    currVol+=(Float32).07;
    SetVolume(currVol, theDevice);
}


- (void)lowerVolume {
    AudioDeviceID theDevice = GetDefaultOutputDevice();
    
    Float32 currVol = GetVolumeScalar(theDevice, false, 1);
    NSLog(@"current volume %f",currVol);
    currVol-=(Float32).07;
    SetVolume(currVol, theDevice);
    
    
}

void SetVolume(Float32 theVolume, AudioDeviceID theDevice)
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

Float32 GetVolumeScalar(AudioDeviceID inDevice, bool inIsInput, UInt32 inChannel)
{
    Float32 theAnswer = 0;
    UInt32 theSize = sizeof(Float32);
    AudioObjectPropertyScope theScope = inIsInput ? kAudioDevicePropertyScopeInput : kAudioDevicePropertyScopeOutput;
    AudioObjectPropertyAddress theAddress = { kAudioDevicePropertyVolumeScalar,
        theScope,
        inChannel };
    
    OSStatus theError = AudioObjectGetPropertyData(inDevice,
                                                   &theAddress,
                                                   0,
                                                   NULL,
                                                   &theSize,
                                                   &theAnswer);
    // handle errors
    
    return theAnswer;
}



AudioDeviceID GetDefaultOutputDevice()
{
    AudioDeviceID theAnswer = 0;
    UInt32 theSize = sizeof(AudioDeviceID);
    AudioObjectPropertyAddress theAddress = { kAudioHardwarePropertyDefaultOutputDevice,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMaster };
    
    OSStatus theError = AudioObjectGetPropertyData(kAudioObjectSystemObject,
                                                   &theAddress,
                                                   0,
                                                   NULL,
                                                   &theSize,
                                                   &theAnswer);
    // handle errors
    
    return theAnswer;
}



/*
 This method actually starts the accept socket. It is the first thing called by
 the run-loop.
 */
- (void) acceptOnPortString:(NSString *)str
{
    // AsyncSocket requires a run-loop.
//    NSAssert ([[NSRunLoop currentRunLoop] currentMode] != nil, @"Run loop is not running");
    
    UInt16 port = [str intValue];
    GCDAsyncSocket *acceptor = (GCDAsyncSocket *)[sockets objectAtIndex:0];
    
    NSError *err = nil;
    if ([acceptor acceptOnPort:port error:&err])
        NSLog (@"Waiting for connections on port %u.", port);
    else
    {
        // If you get a generic CFSocket error, you probably tried to use a port
        // number reserved by the operating system.
        
        NSLog (@"Cannot accept connections on port %u. Error domain %@ code %d (%@). Exiting.", port, [err domain], [err code], [err localizedDescription]);
        exit(1);
    }
}


/*
 This will be called whenever AsyncSocket is about to disconnect. In Echo Server,
 it does not do anything other than report what went wrong (this delegate method
 is the only place to get that information), but in a more serious app, this is
 a good place to do disaster-recovery by getting partially-read data. This is
 not, however, a good place to do cleanup. The socket must still exist when this
 method returns.
 
 I do not implement -onSocketDidDisconnect:. Normally, that is where you would
 release the disconnected socket and perform housekeeping, but I am satisfied
 to leave the disconnected socket instances alone until Echo Server quits.
 */

- (void)socketDidDisconnect:(GCDAsyncSocket *)sock withError:(NSError *)err
{
    if (sock != listenSocket)
    {
        dispatch_async(dispatch_get_main_queue(), ^{
            @autoreleasepool {
                
                NSLog(@"Client Disconnected");
                
            }
        });
        
        @synchronized(sockets)
        {
            [sockets removeObject:sock];
        }
    }
}

-(void) socket:(GCDAsyncSocket *)sock willDisconnectWithError:(NSError *)err
{
    if (err != nil)
        NSLog (@"Socket will disconnect. Error domain %@, code %d (%@).",
               [err domain], [err code], [err localizedDescription]);
    else
        NSLog (@"Socket will disconnect. No error.");
}


/*
 This method is called when a connection is accepted and a new socket is created.
 This is a good place to perform housekeeping and re-assignment -- assigning an
 controller for the new socket, or retaining it. Here, I add it to the array of
 sockets. However, the new socket has not yet connected and no information is
 available about the remote socket, so this is not a good place to screen incoming
 connections. Use onSocket:didConnectToHost:port: for that.
 */
- (void)socket:(GCDAsyncSocket *)sock didAcceptNewSocket:(GCDAsyncSocket *)newSocket
{
    NSLog (@"Socket %lu accepting connection.", (unsigned long)[sockets count]);
    
    @synchronized(sockets)
    {
        [sockets addObject:newSocket];
    }
    
    NSString *welcomeMsg = @"Welcome to the AsyncSocket Echo Server\r\n";
    NSData *welcomeData = [welcomeMsg dataUsingEncoding:NSUTF8StringEncoding];
    
    [newSocket writeData:welcomeData withTimeout:-1 tag:0];
    // Start reading.
    [newSocket readDataToData:[GCDAsyncSocket CRLFData] withTimeout:-1 tag:[sockets indexOfObject:newSocket]];
}


/*
 At this point, the new socket is ready to use. This is where you can screen the
 remote socket or find its DNS name (the host parameter is just an IP address).
 This is also where you should set up your initial read or write request, unless
 you have a particular reason for delaying it.
 */
- (void)socket:(GCDAsyncSocket *)sock didConnectToHost:(NSString *)host port:(uint16_t)port
{
    NSLog (@"Socket %lu successfully accepted connection from %@ %u.", (unsigned long)[sockets indexOfObject:sock], host, port);
    
    // In Echo Server, each packet consists of a line of text, delimited by "\n".
    // This is not a technique you should use. I do not know what "\n" actually
    // means in terms of bytes. It could be CR, LF, or CRLF.
    //
    // In your own networking protocols, you must be more explicit. AsyncSocket
    // provides byte sequences for each line ending. These are CRData, LFData,
    // and CRLFData. You should use one of those instead of "\n".
    NSString *welcomeMsg = @"Welcome to the AsyncSocket Echo Server\r\n";
    NSData *welcomeData = [welcomeMsg dataUsingEncoding:NSUTF8StringEncoding];
    
    [sock writeData:welcomeData withTimeout:-1 tag:0];
    // Start reading.
    [sock readDataToData:[GCDAsyncSocket CRLFData] withTimeout:-1 tag:[sockets indexOfObject:sock]];
}


/*
 This method is called whenever a packet is read. In Echo Server, a packet is
 simply a line of text, and it is transmitted to the connected Echo clients.
 Once you have dealt with the incoming packet, you should set up another read or
 write request, or -- unless there are other requests queued up -- AsyncSocket
 will sit idle.
 */
- (void)socket:(GCDAsyncSocket *)sock didReadData:(NSData *)data withTag:(long)tag
{
    NSString *str = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    
    // Print string.
    NSString *trimStr = [str stringByTrimmingCharactersInSet:[NSCharacterSet controlCharacterSet]];
    NSLog (@"Socket %ld sent text \"%@\".", tag, trimStr);
    
    if ([trimStr isEqualToString:@"raise"]) {
        [self raiseVolume];
    }
    if ([trimStr isEqualToString:@"lower"]) {
        [self lowerVolume];
    }
    
    // Forward string to other sockets.
    int i; for (i = 1; i < [sockets count]; ++i)
        [(GCDAsyncSocket *)[sockets objectAtIndex:i] writeData:data withTimeout:-1 tag:i];
    
    // Read more from this socket.
    [sock readDataToData:[GCDAsyncSocket CRLFData] withTimeout:-1 tag:tag];
}


/*
 This method is called when AsyncSocket has finished writing something. Echo
 Server does not need to do anything after writing, but your own app might need
 to wait for a command from the remote application, or begin writing the next
 packet.
 */
-(void) socket:(GCDAsyncSocket *)sock didWriteDataWithTag:(long)tag
{
    NSLog (@"Wrote to socket %d.", tag);
}

@end
