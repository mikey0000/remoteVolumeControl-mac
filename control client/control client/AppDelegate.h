//
//  AppDelegate.h
//  control client
//
//  Created by Michael Arthur on 14/08/14.
//  Copyright (c) 2014 Michael Arthur. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "AsyncSocket.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>


@end

@interface Echo : NSObject <GCDAsyncSocketDelegate>
{
    GCDAsyncSocket *socket;
}
-(id)init;
-(void)dealloc;
-(void)runLoop;
-(void)connectToHost: (NSString *) host port: (UInt16) port;
-(void)readFromServer;
-(void)writeToServer:(NSString *) message;
//-(void)onSocket:(GCDAsyncSocket *)sock willDisconnectWithError:(NSError *)err;
//-(void)onSocketDidDisconnect:(GCDAsyncSocket *)sock;
//-(void)onSocket:(GCDAsyncSocket *)sock didConnectToHost:(NSString *)host port:(UInt16)port;
//-(void)onSocket:(GCDAsyncSocket *)sock didReadData:(NSData*)data withTag:(long)t;
@end

@implementation Echo : NSObject

/*
 This method creates the socket. Echo reuses this one socket throughout its life.
 Echo also sets up the input. While a command-line app is waiting for input, it
 is usually blocked; I make the input non-blocking so that the run-loop remains
 active.
 */
- (id)init
{
    self = [super init];
    
    // Create socket.
    NSLog (@"Creating socket.");
    socket = [[GCDAsyncSocket alloc] initWithDelegate:self delegateQueue:dispatch_get_main_queue()];
    return self;
}


/*
 I release allocated resources here, including the socket. The socket will close
 any connection before releasing itself, but it will not need to. I explicitly
 close any connections in the "quit" command handler.
 */


/*
 Echo spends one second handling any run-loop activity (i.e. socket activity)
 and then comes up for air to check if any new commands have been entered and,
 if so, executing them. Wash, rinse, repeat.
 
 Note the use of the shouldExitLoop flag to control when the run-loop ends and
 the app quits. I could have just called exit(), but this allows the app to clean
 up after itself properly. You should use a similar technique if you create a
 thread for socket activity and processing.
 */

-(void)writeToServer:(NSString*) message {
    NSMutableData *data = [message dataUsingEncoding:NSUTF8StringEncoding];
    [data appendData:[GCDAsyncSocket CRLFData]];
    [socket writeData: data withTimeout:-1 tag:0];
    
    //[[NSRunLoop currentRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:1.0]];
    
    
    
}

//- (void)socket:(GCDAsyncSocket *)sock didWriteDataWithTag:(long)tag
//{
//    
//}

//- (void)runLoop
//{
//    int n;
//    NSData *data = [@"Hello world\n" dataUsingEncoding:NSASCIIStringEncoding];
//    
//    for( n=0; n<5; n++)
//    {
//        [socket writeData: data withTimeout:-1 tag:0];
//        
//        [[NSRunLoop currentRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:1.0]];
//    }
//}

-(void) connectToHost: (NSString *) host port: (UInt16) port
{
    @try
    {
        NSError *err;
        
        if ([socket connectToHost:host onPort:port error:&err])
            NSLog (@"Connecting to %@ port %u.", host, port);
        else
            NSLog (@"Couldn't connect to %@ port %u (%@).", host, port, err);
    }
    @catch (NSException *exception)
    {
        NSLog ([exception reason]);
    }
}


/*
 This method simply abstracts the read-from-server operation. It is called
 from -onSocket:didReadData:withTag: to set up another read operation. If it did
 not set up another read operation, AsyncSocket would not do anything with any
 further packets from Echo Server.
 
 You should not use "\n" as a packet delimiter in your own code. I explain why
 in EchoServerMain.c.
 */
- (void)readFromServer
{
    [socket readDataToData:[GCDAsyncSocket CRLFData] withTimeout:-1 tag:0];
}

#pragma mark AsyncSocket Delegate Methods


/*
 This will be called whenever AsyncSocket is about to disconnect. In Echo Server,
 it does not do anything other than report what went wrong (this delegate method
 is the only place to get that information), but in a more serious app, this is
 a good place to do disaster-recovery by getting partially-read data. This is
 not, however, a good place to do cleanup. The socket must still exist when this
 method returns.
 */
-(void) socket:(GCDAsyncSocket *)sock willDisconnectWithError:(NSError *)err
{
    if (err != nil)
        NSLog (@"Socket will disconnect. Error domain %@, code %d (%@).",
               [err domain], [err code], [err localizedDescription]);
    else
        NSLog (@"Socket will disconnect. No error.");
}


/*
 Normally, this is the place to release the socket and perform the appropriate
 housekeeping and notification. But I intend to re-use this same socket for
 other connections, so I do nothing.
 */
-(void) socketDidDisconnect:(GCDAsyncSocket *)sock
{
    NSLog (@"Disconnected.");
}


-(void) dealloc
{
    
}

/*
 This method is called when Echo has connected to Echo Server. I immediately
 wait for incoming data from the server, but I already have two write requests
 queued up (from -prepareHello), and will also be sending data when
 the user gives me some to send.
 */
- (void)socket:(GCDAsyncSocket *)sock didConnectToHost:(NSString *)host port:(uint16_t)port;
{
    NSLog (@"Connected to %@ %u.", host, port);
    [self readFromServer];
    
    NSString *welcomeMsg = @"Welcome to the AsyncSocket client\r\n";
    NSData *welcomeData = [welcomeMsg dataUsingEncoding:NSUTF8StringEncoding];
    
    [sock writeData:welcomeData withTimeout:-1 tag:0];
}


/*
 This method is called when Echo has finished reading a packet from Echo Server.
 It prints it out and immediately calls -readFromServer, which will queue up a
 read operation, waiting for the next packet.
 
 You'll note that I do not implement -onSocket:didWriteDataWithTag:. That is
 because Echo does not care about the data once it is transmitted. AsyncSocket
 will still send the data, but will not notify Echo when that it done.
 */
-(void) socket:(GCDAsyncSocket *)sock didReadData:(NSData*)data withTag:(long)t
{
    NSString *str = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    NSLog(@"%@", str );
    [self readFromServer];
}


@end
