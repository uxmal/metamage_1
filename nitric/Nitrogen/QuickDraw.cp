// QuickDraw.cp

#ifndef NITROGEN_QUICKDRAW_H
#include "Nitrogen/QuickDraw.h"
#endif

#ifndef NUCLEUS_ONLYONCE_H
#include "Nucleus/OnlyOnce.h"
#endif

#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif

#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

#if TARGET_RT_MAC_MACHO || TARGET_API_MAC_OSX
#ifndef NITROGEN_CGDIRECTDISPLAY_H
#include "Nitrogen/CGDirectDisplay.h"
#endif
#endif

namespace Nitrogen
{
	
	GrafPtr GetPort()
	{
		::GrafPtr port;
		::GetPort( &port );
		
		return port;
	}
	
	bool QDSwapPort( CGrafPtr newPort, CGrafPtr& oldPort )
	{
	#if TARGET_API_MAC_CARBON
		
		return ::QDSwapPort( newPort, &oldPort );
		
	#else
		
		oldPort = GetPort();
		SetPort( newPort );
		
		return oldPort != newPort;
		
	#endif
	}
	
	CGrafPtr QDSwapPort( CGrafPtr newPort )
	{
		CGrafPtr oldPort;
		(void) QDSwapPort( newPort, oldPort );
		
		return oldPort;
	}
	
	Nucleus::Owned< RgnHandle > GetClip()
	{
		Nucleus::Owned< RgnHandle > result = NewRgn();
		::GetClip( result.Get() );
		
		return result;
	}
	
	void MacSetCursor( ResID id )
	{
		MacSetCursor( MacGetCursor( id ) );
	}
	
	PenState GetPenState()
	{
		PenState penState;
		::GetPenState( &penState );
		
		return penState;
	}
	
	Rect MacOffsetRect( const Rect& r, short dh, short dv )
	{
		return MacSetRect( r.left   + dh,
		                   r.top    + dv,
		                   r.right  + dh,
		                   r.bottom + dv );
	}
	
	Rect MacInsetRect( const Rect& r, short dh, short dv )
	{
		return MacSetRect( r.left   + dh,
		                   r.top    + dv,
		                   r.right  - dh,
		                   r.bottom - dv );
	}
	
	SectRect_Result SectRect( const Rect& a, const Rect& b )
	{
		SectRect_Result result;
		result.intersected = ::SectRect( &a, &b, &result.intersection );
		
		return result;
	}
	
	Rect MacUnionRect( const Rect& a, const Rect& b )
	{
		Rect result;
		::MacUnionRect( &a, &b, &result );
		
		return result;
	}
	
	Nucleus::Owned< RgnHandle > NewRgn(void)
	{
		RgnHandle result = ::NewRgn();
		
		if ( result == NULL )
		{
			throw MemFullErr();
		}
		
		return Nucleus::Owned< RgnHandle >::Seize( result );
	}
	
	Nucleus::Owned< RgnHandle > MacCopyRgn( RgnHandle srcRgn )
	{
		Nucleus::Owned< RgnHandle > result = NewRgn();
		::MacCopyRgn( srcRgn, result );
		
		return result;
	}
	
	RgnHandle RectRgn( RgnHandle region, const Rect& rect )
	{
		::RectRgn( region, &rect );
		
		return region;
	}
	
	Nucleus::Owned< RgnHandle > RectRgn( const Rect& rect )
	{
		Nucleus::Owned< RgnHandle > result = NewRgn();
		::RectRgn( result, &rect );
		
		return result;
	}
	
	Nucleus::Owned< RgnHandle > SectRgn( RgnHandle a, RgnHandle b )
	{
		Nucleus::Owned< RgnHandle > result = NewRgn();
		::SectRgn( a, b, result );
		
		return result;
	}
	
	Nucleus::Owned< RgnHandle > MacUnionRgn( RgnHandle a, RgnHandle b )
	{
		Nucleus::Owned< RgnHandle > result = NewRgn();
		::MacUnionRgn( a, b, result );
		
		return result;
	}
	
	Nucleus::Owned< RgnHandle > DiffRgn( RgnHandle a, RgnHandle b )
	{
		Nucleus::Owned< RgnHandle > result = NewRgn();
		::DiffRgn( a, b, result );
		
		return result;
	}
	
	Nucleus::Owned< RgnHandle > MacXorRgn( RgnHandle a, RgnHandle b )
	{
		Nucleus::Owned< RgnHandle > result = NewRgn();
		::MacXorRgn( a, b, result );
		
		return result;
	}
	
	void ScrollRect( const Rect& r, short dh, short dv, RgnHandle updateRgn )
	{
		::ScrollRect( &r, dh, dv, updateRgn );
	}
	
	Nucleus::Owned< RgnHandle > ScrollRect( const Rect& r, short dh, short dv )
	{
		Nucleus::Owned< RgnHandle > region = NewRgn();
		ScrollRect( r, dh, dv, region );
		
		return region;
	}
	
	void CopyBits( const BitMap*  srcBits,
	               const BitMap*  dstBits,
	               const Rect&    srcRect,
	               const Rect&    dstRect,
	               TransferMode   mode,
	               RgnHandle      maskRgn )
	{
		::CopyBits( srcBits, dstBits, &srcRect, &dstRect, mode, maskRgn );
	}
	
	Point LocalToGlobal( Point point )
	{
		::LocalToGlobal( &point );
		
		return point;
	}
	
	Rect LocalToGlobal( const Rect& rect )
	{
		return MacSetRect( LocalToGlobal( SetPt( rect.left,  rect.top    ) ),
		                   LocalToGlobal( SetPt( rect.right, rect.bottom ) ) );
	}
	
	Point GlobalToLocal( Point point )
	{
		::GlobalToLocal( &point );
		
		return point;
	}
	
	Rect GlobalToLocal( const Rect& rect )
	{
		return MacSetRect( GlobalToLocal( SetPt( rect.left,  rect.top    ) ),
		                   GlobalToLocal( SetPt( rect.right, rect.bottom ) ) );
	}
	
	Point AddPt( Point a, Point b )
	{
		return SetPt( a.h + b.h,
		              a.v + b.v );
	}
	
	Point SubPt( Point a, Point b )
	{
		return SetPt( a.h - b.h,
		              a.v - b.v );
	}
	
	RGBColor GetCPixel( short h, short v )
	{
		RGBColor result;
		::GetCPixel( h, v, &result );
		
		return result;
	}
	
	RGBColor GetForeColor()
	{
		RGBColor result;
		::GetForeColor( &result );
		
		return result;
	}
	
	RGBColor GetBackColor()
	{
		RGBColor result;
		::GetBackColor( &result );
		
		return result;
	}
	
	void QDError()
	{
		Nucleus::OnlyOnce< RegisterQuickDrawErrors >();
		
		ThrowOSStatus( ::QDError() );
	}
	
	CursHandle MacGetCursor( ResID id )
	{
		return CheckResource( ::MacGetCursor( id ) );
	}
	
#if OPAQUE_TOOLBOX_STRUCTS
	
	Rect GetPortBounds( CGrafPtr port )
	{
		Rect bounds;
		return *( ::GetPortBounds( port, &bounds ) );
	}
	
#endif
	
	RGBColor GetPortForeColor( CGrafPtr port )
	{
	#if TARGET_API_MAC_CARBON
		
		RGBColor result;
		::GetPortForeColor( port, &result );
		
		return result;
		
	#else
		
		return ::CGrafPtr( port )->rgbFgColor;
		
	#endif
	}
	
	RGBColor GetPortBackColor( CGrafPtr port )
	{
	#if TARGET_API_MAC_CARBON
		
		RGBColor result;
		::GetPortBackColor( port, &result );
		
		return result;
		
	#else
		
		return ::CGrafPtr( port )->rgbBkColor;
		
	#endif
	}
	
	RgnHandle GetPortVisibleRegion( CGrafPtr port, RgnHandle region )
	{
	#if OPAQUE_TOOLBOX_STRUCTS
		
		return ::GetPortVisibleRegion( port, region );
		
	#else
		
		::MacCopyRgn( ::GrafPtr( port )->visRgn, region );
		
		return region;
		
	#endif
	}
	
	Nucleus::Owned< RgnHandle > GetPortVisibleRegion( CGrafPtr port )
	{
		Nucleus::Owned< RgnHandle > region = NewRgn();
		(void)Nitrogen::GetPortVisibleRegion( port, region );
		
		return region;
	}
	
	RgnHandle GetPortClipRegion( CGrafPtr port, RgnHandle region )
	{
	#if OPAQUE_TOOLBOX_STRUCTS
		
		return ::GetPortClipRegion( port, region );
		
	#else
		
		::MacCopyRgn( ::GrafPtr( port )->clipRgn, region );
		
		return region;
		
	#endif
	}
	
	Nucleus::Owned< RgnHandle > GetPortClipRegion( CGrafPtr port )
	{
		Nucleus::Owned< RgnHandle > region = NewRgn();
		Nitrogen::GetPortClipRegion( port, region );
		
		return region;
	}
	
	Point GetPortPenSize( CGrafPtr port )
	{
	#if OPAQUE_TOOLBOX_STRUCTS
		
		Point result;
		(void)::GetPortPenSize( port, &result );
		
		return result;
		
	#else
		
		return ::GrafPtr( port )->pnSize;
		
	#endif
	}
	
	bool IsPortColor( CGrafPtr port )
	{
	#if TARGET_API_MAC_CARBON
		
		return ::IsPortColor( port );
		
	#else
		
		// Taken from QISA/MoreIsBetter/MoreQuickDraw.cp
		return ::CGrafPtr( port )->portVersion < 0;
		
	#endif
	}
	
	void SetPortClipRegion( CGrafPtr port, RgnHandle clipRgn )
	{
	#if OPAQUE_TOOLBOX_STRUCTS
		
		::SetPortClipRegion( port, clipRgn );
		
	#else
		
		MacCopyRgn( clipRgn, ::GrafPtr( port )->clipRgn );
		
	#endif
	}
	
	void SetPortPenSize( CGrafPtr port, Point penSize )
	{
	#if OPAQUE_TOOLBOX_STRUCTS
		
		::SetPortPenSize( port, penSize );
		
	#else
		
		::GrafPtr( port )->pnSize = penSize;
		
	#endif
	}
	
#if OPAQUE_TOOLBOX_STRUCTS
	
	template < class Type >
	Type& QDGlobalsVar
	(
	#if !TARGET_RT_MAC_MACHO
		pascal
	#endif
		Type* (getter)(Type*)
	)
	{
		static Type var;
		return *getter( &var );
	}
	
	// Thread-safety note:
	//
	// GetQDGlobalsScreenBits() and GetQDGlobalsArrow() should be okay,
	// since two simultaneous accesses would be copying the same data.
	// The Pattern-returning functions are not thread-safe.
	
	const BitMap&  GetQDGlobalsScreenBits()  { return QDGlobalsVar( ::GetQDGlobalsScreenBits ); }
	const Cursor&  GetQDGlobalsArrow()       { return QDGlobalsVar( ::GetQDGlobalsArrow      ); }
	const Pattern& GetQDGlobalsDarkGray()    { return QDGlobalsVar( ::GetQDGlobalsDarkGray   ); }
	const Pattern& GetQDGlobalsLightGray()   { return QDGlobalsVar( ::GetQDGlobalsLightGray  ); }
	const Pattern& GetQDGlobalsGray()        { return QDGlobalsVar( ::GetQDGlobalsGray       ); }
	const Pattern& GetQDGlobalsBlack()       { return QDGlobalsVar( ::GetQDGlobalsBlack      ); }
	const Pattern& GetQDGlobalsWhite()       { return QDGlobalsVar( ::GetQDGlobalsWhite      ); }
	
#endif
	
	Nucleus::Owned< CGrafPtr > CreateNewPort()
	{
	#if ACCESSOR_CALLS_ARE_FUNCTIONS
		
		CGrafPtr result = ::CreateNewPort();
		
	#else
		
		Nucleus::Owned< Ptr > portMem = NewPtr( sizeof (::CGrafPort) );
		
		::OpenCPort( Ptr_Cast< ::CGrafPort >( portMem.Get() ) );
		
		CGrafPtr result( Ptr_Cast< ::CGrafPort >( portMem.Release() ) );
		
	#endif
		
		return Nucleus::Owned< CGrafPtr >::Seize( result );
	}
	
	namespace Private
	{
		
		void DisposePort( CGrafPtr port )
		{
		#if ACCESSOR_CALLS_ARE_FUNCTIONS
			
			::DisposePort( port );
			
		#else
			
			::CloseCPort( port );
			
			DisposePtr( Nucleus::Owned< Ptr >::Seize( ::CGrafPtr( port ) ) );
			
		#endif
		}
		
	}
	
#if TARGET_API_MAC_OSX
	
	// Declared in Nitrogen/CGDirectDisplay.h, since it depends on CGDirectDisplayID
	
	Nucleus::Owned< CGrafPtr > CreateNewPortForCGDisplayID( CGDirectDisplayID display )
	{
		return Nucleus::Owned< CGrafPtr >::Seize
		(
			::CreateNewPortForCGDisplayID( reinterpret_cast< unsigned long >( display ) )
		);
	}
	
#endif
	
	void RegisterQuickDrawErrors()
	{
		RegisterMemoryManagerErrors();
		
		// CopyBits couldn't allocate required temporary memory
		RegisterOSStatus< -143                    >();
		
		// Ran out of stack space while drawing polygon
		RegisterOSStatus< -144                    >();
		
		RegisterOSStatus< noMemForPictPlaybackErr >();
		RegisterOSStatus< rgnOverflowErr          >();
		RegisterOSStatus< pixMapTooDeepErr        >();
		RegisterOSStatus< insufficientStackErr    >();
		RegisterOSStatus< cMatchErr               >();
		RegisterOSStatus< cTempMemErr             >();
		RegisterOSStatus< cNoMemErr               >();
		RegisterOSStatus< cRangeErr               >();
		RegisterOSStatus< cProtectErr             >();
		RegisterOSStatus< cDevErr                 >();
		RegisterOSStatus< cResErr                 >();
		RegisterOSStatus< cDepthErr               >();
		RegisterOSStatus< rgnTooBigErr            >();
		RegisterOSStatus< updPixMemErr            >();
		RegisterOSStatus< pictInfoVersionErr      >();
		RegisterOSStatus< pictInfoIDErr           >();
		RegisterOSStatus< pictInfoVerbErr         >();
		RegisterOSStatus< cantLoadPickMethodErr   >();
		RegisterOSStatus< colorsRequestedErr      >();
		RegisterOSStatus< pictureDataErr          >();
	}
  
}

