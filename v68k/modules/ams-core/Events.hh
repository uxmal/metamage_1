/*
	Events.hh
	---------
*/

#ifndef EVENTS_HH
#define EVENTS_HH

struct EventRecord;
struct MacRegion;
struct Point;

pascal unsigned char GetNextEvent_patch( unsigned short  eventMask,
                                         EventRecord*    event );

pascal unsigned char WaitNextEvent_patch( unsigned short  eventMask,
                                          EventRecord*    event,
                                          unsigned long   sleep,
                                          MacRegion**     mouseRgn );

pascal void GetMouse_patch( Point* loc );

pascal char Button_patch();
pascal char StillDown_patch();

#endif
