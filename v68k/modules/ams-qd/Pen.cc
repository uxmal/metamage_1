/*
	Pen.cc
	------
*/

#include "Pen.hh"

// iota
#include "iota/swap.hh"

// ams-common
#include "QDGlobals.hh"


static inline
short min( short a, short b )
{
	return b < a ? b : a;
}

static inline
short max( short a, short b )
{
	return a > b ? a : b;
}

static void even_dexter( Point a, UInt16 n )
{
	Rect r = { a.v, a.h, a.v + 1, a.h + 1 };
	
	PaintRect( &r );
	
	while ( n > 0 )
	{
		++r.top;
		++r.left;
		++r.bottom;
		++r.right;
		
		PaintRect( &r );
		
		--n;
	}
}

static void even_sinister( Point a, UInt16 n )
{
	Rect r = { a.v, a.h, a.v + 1, a.h + 1 };
	
	PaintRect( &r );
	
	while ( n > 0 )
	{
		++r.top;
		--r.left;
		++r.bottom;
		--r.right;
		
		PaintRect( &r );
		
		--n;
	}
}

pascal void StdLine_patch( Point newPt )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	Point pnLoc = thePort->pnLoc;
	
	if ( pnLoc.v > newPt.v )
	{
		using iota::swap;
		
		swap( pnLoc, newPt );
	}
	
	Rect line;
	
	line.top    = min( pnLoc.v, newPt.v );
	line.left   = min( pnLoc.h, newPt.h );
	line.bottom = max( pnLoc.v, newPt.v );
	line.right  = max( pnLoc.h, newPt.h );
	
	if ( line.top == line.bottom  ||  line.left == line.right )
	{
		line.bottom += thePort->pnSize.v;
		line.right  += thePort->pnSize.h;
		
		PaintRect( &line );
		
		return;
	}
	
	const UInt16 descent = line.bottom - line.top;
	const UInt16 advance = line.right - line.left;
	
	if ( advance == descent )
	{
		if ( newPt.h >= pnLoc.h )
		{
			even_dexter( pnLoc, descent );
		}
		else
		{
			even_sinister( pnLoc, descent );
		}
	}
}

pascal void HidePen_patch()
{
	GrafPtr thePort = *get_addrof_thePort();
	
	--thePort->pnVis;
}

pascal void ShowPen_patch()
{
	GrafPtr thePort = *get_addrof_thePort();
	
	++thePort->pnVis;
}

pascal void GetPen_patch( Point* pt )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	*pt = thePort->pnLoc;
}

pascal void GetPenState_patch( PenState* state )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	*state = *(const PenState*) &thePort->pnLoc;
}

pascal void SetPenState_patch( const PenState* state )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	*(PenState*) &thePort->pnLoc = *state;
}

pascal void PenSize_patch( short width, short height )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	thePort->pnSize.v = height;
	thePort->pnSize.h = width;
}

pascal void PenMode_patch( short mode )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	thePort->pnMode = mode;
}

pascal void PenPat_patch( const Pattern* pat )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	thePort->pnPat = *pat;
}

pascal void PenNormal_patch()
{
	QDGlobals& qd = get_QDGlobals();
	
	GrafPtr thePort = *get_addrof_thePort();
	
	thePort->pnSize.v = 1;
	thePort->pnSize.h = 1;
	
	thePort->pnMode = patCopy;
	thePort->pnPat  = qd.black;
}

pascal void MoveTo_patch( short h, short v )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	thePort->pnLoc.v = v;
	thePort->pnLoc.h = h;
}

pascal void Move_patch( short dh, short dv )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	thePort->pnLoc.v += dv;
	thePort->pnLoc.h += dh;
}

pascal void LineTo_patch( short h, short v )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	if ( thePort->pnVis >= 0 )
	{
		StdLine( *(const Point*) &v );
	}
	
	thePort->pnLoc.v = v;
	thePort->pnLoc.h = h;
}

pascal void Line_patch( short dh, short dv )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	Point pnLoc = thePort->pnLoc;
	
	LineTo_patch( pnLoc.h + dh, pnLoc.v + dv );
}
