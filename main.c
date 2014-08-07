// Philippe Groarke, 2014


/*          DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
                    Version 2, December 2004

 Copyright (C) 2004 Sam Hocevar <sam@hocevar.net>

 Everyone is permitted to copy and distribute verbatim or modified
 copies of this license document, and changing it is allowed as long
 as the name is changed.

            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

  0. You just DO WHAT THE FUCK YOU WANT TO. */


#import <ApplicationServices/ApplicationServices.h>
//#import <Carbon/Carbon.h>
#include <time.h>

struct timeval timer, currentTime;
const double clickInterval = 0.137;
const int scrollUp = 1;
const int scrollDown = -1;
//const UniChar upKey[1] = {'p'};

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
        //printf("scrolled up\n");

        CGEventRef eventUp = CGEventCreateKeyboardEvent(NULL, 0x23, true);
        CGEventPost(kCGHIDEventTap, eventUp);
        eventUp = CGEventCreateKeyboardEvent(NULL, 0x23, false);
        CGEventPost(kCGHIDEventTap, eventUp);
        CFRelease(eventUp);

        // TESTS, didnt work!
        //CGEventSetType(event, kCGEventKeyDown); // Change event type.
        //CGEventSetIntegerValueField(event, kCGKeyboardEventKeycode, kVK_ANSI_P);

        //CGEventKeyboardSetUnicodeString(event, sizeof(upKey), upKey);
    }

    else if (scrollDirection == scrollDown)
    {
        //printf("scrolled down\n");

        CGEventRef eventDown = CGEventCreateKeyboardEvent(NULL, 0x25, true);
        CGEventPost(kCGHIDEventTap, eventDown);
        eventDown = CGEventCreateKeyboardEvent(NULL, 0x25, false);
        CGEventPost(kCGHIDEventTap, eventDown);
        CFRelease(eventDown);
    }

    // DEBUG
    // printf("%u\n", (int)event);
    // printf("%i\n", CGEventGetIntegerValueField(event, kCGScrollWheelEventDeltaAxis1));
    // printf("%.6lf\n", timeDifference);

    gettimeofday(&timer, NULL);
    //return event;
    return NULL; // Stop the event!
}

int main(int argc, char* argv[]) {
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
    
    return 0;
}