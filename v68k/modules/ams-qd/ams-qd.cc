/*
	ams-qd.cc
	---------
*/

// Mac OS
#ifndef __TRAPS__
#include <Traps.h>
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>

// ams-core
#include "CopyBits.hh"
#include "Cursor.hh"
#include "Fixed.hh"
#include "Fonts.hh"
#include "GrafPorts.hh"
#include "InitGraf.hh"
#include "Ovals.hh"
#include "QDUtils.hh"
#include "Pen.hh"
#include "Rects.hh"
#include "Region-ops.hh"
#include "Regions.hh"
#include "RoundRects.hh"
#include "StrUtils.hh"
#include "Text.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define PROGRAM  "ams-qd"

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


void* toolbox_trap_table[] : 3 * 1024;

#define TBTRAP( Proc )  (toolbox_trap_table[ _##Proc & 0x03FF ] = &Proc##_patch)

enum
{
	_SetPortBits = _SetPBits,
	_SetRectRgn  = _SetRecRgn,
};


static void install_QuickDraw()
{
	TBTRAP( InitCursor );  // A850
	TBTRAP( SetCursor  );  // A851
	TBTRAP( HideCursor );  // A852
	TBTRAP( ShowCursor );  // A853
	
	TBTRAP( Random );  // A861
	
	TBTRAP( StuffHex );  // A866
	
	TBTRAP( FixMul   );  // A868
	TBTRAP( FixRatio );  // A869
	TBTRAP( HiWord   );  // A86A
	TBTRAP( LoWord   );  // A86B
	TBTRAP( FixRound );  // A86C
	
	TBTRAP( InitPort    );  // A86D
	TBTRAP( InitGraf    );  // A86E
	TBTRAP( OpenPort    );  // A86F
	
	TBTRAP( LocalToGlobal );  // A870
	TBTRAP( GlobalToLocal );  // A871
	
	TBTRAP( SetPort     );  // A873
	TBTRAP( GetPort     );  // A874
	TBTRAP( SetPortBits );  // A875
	TBTRAP( PortSize    );  // A876
	TBTRAP( MovePortTo  );  // A877
	TBTRAP( SetOrigin   );  // A878
	TBTRAP( SetClip     );  // A879
	TBTRAP( GetClip     );  // A87A
	TBTRAP( ClipRect    );  // A87B
	TBTRAP( BackPat     );  // A87C
	TBTRAP( ClosePort   );  // A87D
	
	TBTRAP( StdText     );  // A882
	TBTRAP( DrawChar    );  // A883
	TBTRAP( DrawString  );  // A884
	TBTRAP( DrawText    );  // A885
	TBTRAP( TextWidth   );  // A886
	TBTRAP( TextFont    );  // A887
	TBTRAP( TextFace    );  // A888
	TBTRAP( TextMode    );  // A889
	TBTRAP( TextSize    );  // A88A
	TBTRAP( GetFontInfo );  // A88B
	TBTRAP( StringWidth );  // A88C
	TBTRAP( CharWidth   );  // A88D
	TBTRAP( SpaceExtra  );  // A88E
	
	TBTRAP( StdLine );  // A890
	TBTRAP( LineTo  );  // A891
	TBTRAP( Line    );  // A892
	TBTRAP( MoveTo  );  // A893
	TBTRAP( Move    );  // A894
	
	TBTRAP( HidePen     );  // A896
	TBTRAP( ShowPen     );  // A897
	TBTRAP( GetPenState );  // A898
	TBTRAP( SetPenState );  // A899
	TBTRAP( GetPen      );  // A89A
	TBTRAP( PenSize     );  // A89B
	TBTRAP( PenMode     );  // A89C
	TBTRAP( PenPat      );  // A89D
	TBTRAP( PenNormal   );  // A89E
	
	TBTRAP( StdRect    );  // A8A0
	TBTRAP( FrameRect  );  // A8A1
	TBTRAP( PaintRect  );  // A8A2
	TBTRAP( EraseRect  );  // A8A3
	TBTRAP( InverRect  );  // A8A4
	TBTRAP( FillRect   );  // A8A5
	TBTRAP( EqualRect  );  // A8A6
	TBTRAP( SetRect    );  // A8A7
	TBTRAP( OffsetRect );  // A8A8
	TBTRAP( InsetRect  );  // A8A9
	TBTRAP( SectRect   );  // A8AA
	TBTRAP( UnionRect  );  // A8AB
	TBTRAP( Pt2Rect    );  // A8AC
	TBTRAP( PtInRect   );  // A8AD
	TBTRAP( EmptyRect  );  // A8AE
	
	TBTRAP( StdRRect       );  // A8AF
	TBTRAP( FrameRoundRect );  // A8B0
	TBTRAP( PaintRoundRect );  // A8B1
	TBTRAP( EraseRoundRect );  // A8B2
	TBTRAP( InverRoundRect );  // A8B3
	TBTRAP( FillRoundRect  );  // A8B4
	
	TBTRAP( StdOval    );  // A8B6
	TBTRAP( FrameOval  );  // A8B7
	TBTRAP( PaintOval  );  // A8B8
	TBTRAP( EraseOval  );  // A8B9
	TBTRAP( InvertOval );  // A8BA
	TBTRAP( FillOval   );  // A8BB
	
	TBTRAP( StdRgn      );  // A8D1
	TBTRAP( FrameRgn    );  // A8D2
	TBTRAP( PaintRgn    );  // A8D3
	TBTRAP( EraseRgn    );  // A8D4
	TBTRAP( InverRgn    );  // A8D5
	TBTRAP( FillRgn     );  // A8D6
	
	TBTRAP( BitMapToRegion );  // A8D7
	
	TBTRAP( NewRgn      );  // A8D8
	TBTRAP( DisposeRgn  );  // A8D9
	TBTRAP( OpenRgn     );  // A8DA
	TBTRAP( CloseRgn    );  // A8DB
	TBTRAP( CopyRgn     );  // A8DC
	TBTRAP( SetEmptyRgn );  // A8DD
	TBTRAP( SetRectRgn  );  // A8DE
	TBTRAP( RectRgn     );  // A8DF
	TBTRAP( OffsetRgn   );  // A8E0
	TBTRAP( InsetRgn    );  // A8E1
	TBTRAP( EmptyRgn    );  // A8E2
	TBTRAP( SectRgn     );  // A8E4
	TBTRAP( UnionRgn    );  // A8E5
	TBTRAP( DiffRgn     );  // A8E6
	TBTRAP( XOrRgn      );  // A8E7
	TBTRAP( PtInRgn     );  // A8E8
	
	TBTRAP( StdBits     );  // A8EB
	TBTRAP( CopyBits    );  // A8EC
	TBTRAP( StdTxMeas   );  // A8ED
	
	TBTRAP( GetPattern  );  // A9B8
	TBTRAP( GetCursor   );  // A9B9
}

static void install_Fonts()
{
	TBTRAP( InitFonts );  // A8FE
	
	TBTRAP( FMSwapFont );  // A901
}

static void install_StrUtils()
{
	TBTRAP( GetString );  // A9BA
}

static asm void module_suspend()
{
	JSR      0xFFFFFFF8
}

int main( int argc, char** argv )
{
	if ( argc > 1 )
	{
		WARN( "no arguments allowed" );
		
		_exit( 1 );
	}
	
	install_QuickDraw();
	install_Fonts();
	install_StrUtils();
	
	module_suspend();  // doesn't return
}
