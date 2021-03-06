/*
	Initialize.cc
	-------------
*/

#include "Pedestal/Initialize.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __DIALOGS__
#include <Dialogs.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif


namespace Pedestal
{
	
	void Init_MacToolbox()
	{
	#if !TARGET_API_MAC_CARBON
		
		::InitGraf( &qd.thePort );
		::InitFonts();
		::InitWindows();
		::InitMenus();
		::TEInit();
		::InitDialogs( NULL );
		
	#endif
		
		::InitCursor();
		// FlushEvents?
	}
	
	void Init_Memory( unsigned moreMasters )
	{
	#if !TARGET_API_MAC_CARBON
		
		::MaxApplZone();
		
	#endif
		
		for ( unsigned i = 0;  i < moreMasters;  ++i )
		{
			::MoreMasters();
		}
	}
	
}
