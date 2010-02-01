// Nitrogen/CGDirectDisplay.h
// --------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2006 by Joshua Juran and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_CGDIRECTDISPLAY_H
#define NITROGEN_CGDIRECTDISPLAY_H

// Mac OS
#ifndef __CGDIRECTDISPLAY__
#include <CGDirectDisplay.h>
#endif
	
// Nitrogen
#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif

//	we need this include in order to instantiate Owned<CGrafPtr>
#ifndef NITROGEN_QUICKDRAW_H
#include "Nitrogen/QuickDraw.h"
#endif

namespace Nitrogen
{
	
	using ::CGDirectDisplayID;
	
  }

namespace Nucleus
  {
	template <> struct Disposer< Nitrogen::CGDirectDisplayID > : public std::unary_function< Nitrogen::CGDirectDisplayID, void >
	{
		void operator()( Nitrogen::CGDirectDisplayID display ) const
		{
			::CGDisplayRelease( display );
		}
	};
  }

namespace Nitrogen
  {
	
	// This belongs in Nitrogen/Quickdraw.h, but it depends on CGDirectDisplayID,
	// which is defined in CGDirectDisplay.h which isn't included by Quickdraw.h,
	// so we move it here so as not to create a new header dependency.
	// (It's defined in Nitrogen/Quickdraw.cp.)
	Nucleus::Owned< CGrafPtr > CreateNewPortForCGDisplayID( CGDirectDisplayID display );
	
	Nucleus::Owned< CGDirectDisplayID > CGDisplayCapture( CGDirectDisplayID display = 0 );
	
	inline void CGDisplayRelease( Nucleus::Owned< CGDirectDisplayID > )  {}
	
}

#endif

