/*
 * To the extent possible under law, Philippe Groarke has waived all
 * copyright and related or neighboring rights to osxscrollreplace.
 *
 * You should have received a copy of the CC0 legalcode along with this
 * work.  If not, see http://creativecommons.org/publicdomain/zero/1.0/
 */


#import <ApplicationServices/ApplicationServices.h>
//#import <Carbon/Carbon.h>
#include <time.h>

struct timeval timer, currentTime;
const double clickInterval = 0.137;
const int scrollUp = 1;
const int scrollDown = -1;


//// BUG ////

CFRunLoopSourceRef runLoopSourceMDOWN;
CFRunLoopSourceRef runLoopSourceMUP;
CFMachPortRef eventTapMDOWN;
CFMachPortRef eventTapMUP;

// Callback for event, this is where you modify it.
CGEventRef MiddleDownCall (CGEventTapProxy proxy, CGEventType type,
    CGEventRef event, void *refcon)
{
    if (CGEventGetIntegerValueField(event, kCGMouseEventButtonNumber) == kCGMouseButtonCenter) {
        //printf("middle down\n");
        //CFRelease(event);
        return NULL;
    }

    return event;
}

// Callback for event, this is where you modify it.
CGEventRef MiddleUpCall (CGEventTapProxy proxy, CGEventType type,
    CGEventRef event, void *refcon)
{
    if (CGEventGetIntegerValueField(event, kCGMouseEventButtonNumber) == kCGMouseButtonCenter) {
        //printf("middle up\n");
        //CFRelease(event);
        return NULL;
    }
    return event;
}


void fixFuckingValveBug()
{
    // FIX FUCKING VALVE SHIT
    CGEventMask mouseMiddleDown = CGEventMaskBit(kCGEventOtherMouseDown);//kCGEventOtherMouseDown);
    CGEventMask mouseMiddleUp = CGEventMaskBit(kCGEventOtherMouseUp);

    // Create event to listen.
    eventTapMDOWN = CGEventTapCreate (
        kCGHIDEventTap,
        kCGHeadInsertEventTap,
        kCGEventTapOptionDefault, // Active, we can modify the event.
        mouseMiddleDown, // Type of event.
        MiddleDownCall, // Callback.
        NULL // Pointer to data passed to user callback.
    );

    // Create loop to run.
    runLoopSourceMDOWN = CFMachPortCreateRunLoopSource (
        kCFAllocatorDefault, // Synonym of NULL
        eventTapMDOWN, // My port
        0 // Priority, ignored
    );

    // Add to current loop.
    CFRunLoopAddSource (
        CFRunLoopGetCurrent(), // The runloop to modify (current one).
        runLoopSourceMDOWN, // My runLoopSource
        kCFRunLoopCommonModes // This adds the default and other modes.
    );


    // Create event to listen.
    eventTapMUP = CGEventTapCreate (
        kCGHIDEventTap,
        kCGHeadInsertEventTap,
        kCGEventTapOptionDefault, // Active, we can modify the event.
        mouseMiddleUp, // Type of event.
        MiddleUpCall, // Callback.
        NULL // Pointer to data passed to user callback.
    );
    if (eventTapMUP == NULL || eventTapMDOWN == NULL)
    {
        printf("eventTap was NULL!\nExiting.\n");
        exit(0);
    }

    // Create loop to run.
    runLoopSourceMUP = CFMachPortCreateRunLoopSource (
        kCFAllocatorDefault, // Synonym of NULL
        eventTapMUP, // My port
        0 // Priority, ignored
    );

    // Add to current loop.
    CFRunLoopAddSource (
        CFRunLoopGetCurrent(), // The runloop to modify (current one).
        runLoopSourceMUP, // My runLoopSource
        kCFRunLoopCommonModes // This adds the default and other modes.
    );

}

////////

// Callback for event, this is where you modify it.
CGEventRef MyEventTapCallBack (CGEventTapProxy proxy, CGEventType type,
    CGEventRef event, void *refcon)
{
    gettimeofday(&currentTime, NULL);
    double t1 = currentTime.tv_sec+(currentTime.tv_usec/1000000.0);
    double t2 = timer.tv_sec+(timer.tv_usec/1000000.0);
    double timeDifference = t1 - t2;

    if (timeDifference < clickInterval)
        return NULL;

    // Modify the current event to be a keyboard button.
    int scrollDirection = CGEventGetIntegerValueField(event, kCGScrollWheelEventDeltaAxis1);
    if (scrollDirection == scrollUp)
    {
        CGEventRef eventUp = CGEventCreateKeyboardEvent(NULL, 0x23, true);
        CGEventPost(kCGHIDEventTap, eventUp);
        eventUp = CGEventCreateKeyboardEvent(NULL, 0x23, false);
        CGEventPost(kCGHIDEventTap, eventUp);
        CFRelease(eventUp);
    }

    else if (scrollDirection == scrollDown)
    {
        CGEventRef eventDown = CGEventCreateKeyboardEvent(NULL, 0x25, true);
        CGEventPost(kCGHIDEventTap, eventDown);
        eventDown = CGEventCreateKeyboardEvent(NULL, 0x25, false);
        CGEventPost(kCGHIDEventTap, eventDown);
        CFRelease(eventDown);
    }

    gettimeofday(&timer, NULL);
    return NULL; // Stop the event!
}


// ENTRY POINT
int main(int argc, char* argv[]) {

    fixFuckingValveBug();

    gettimeofday(&timer, NULL);

    CGEventMask interestedEvents = CGEventMaskBit(kCGEventScrollWheel);

    // Create event to listen.
    CFMachPortRef eventTap;
    eventTap = CGEventTapCreate (
        kCGHIDEventTap,
        kCGHeadInsertEventTap,
        kCGEventTapOptionDefault, // Active, we can modify the event.
        interestedEvents, // Type of event.
        MyEventTapCallBack, // Callback.
        NULL // Pointer to data passed to user callback.
    );
    if (eventTap == NULL)
    {
        printf("eventTap was NULL!\nExiting.\n");
        return 1;
    }

    // Create loop to run.
    CFRunLoopSourceRef runLoopSource;
    runLoopSource = CFMachPortCreateRunLoopSource (
        kCFAllocatorDefault, // Synonym of NULL
        eventTap, // My port
        0 // Priority, ignored
    );

    // Add to current loop.
    CFRunLoopAddSource (
        CFRunLoopGetCurrent(), // The runloop to modify (current one).
        runLoopSource, // My runLoopSource
        kCFRunLoopCommonModes // This adds the default and other modes.
    );

    CFRunLoopRun();

    // Cleanup
    CFRelease(eventTap); // If NULL will crash.
    CFRelease(runLoopSource);

    CFRelease(eventTapMDOWN);
    CFRelease(eventTapMUP);
    CFRelease(runLoopSourceMDOWN);
    CFRelease(runLoopSourceMUP);

    return 0;
}
