/*
	ams-core.cc
	-----------
*/

// Mac OS
#ifndef __TRAPS__
#include <Traps.h>
#endif
#ifndef __EVENTS__
#include <Events.h>
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>

// command
#include "command/get_option.hh"

// gear
#include "gear/parse_decimal.hh"

// ams-core
#include "Debugger.hh"
#include "Desk.hh"
#include "Events.hh"
#include "Files.hh"
#include "Gestalt.hh"
#include "HandleUtils.hh"
#include "Handles.hh"
#include "OSEvents.hh"
#include "OSUtils.hh"
#include "Segments.hh"
#include "SysError.hh"
#include "cursor-core.hh"
#include "options.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define PROGRAM  "ams-core"

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


enum
{
	Opt_linger = 'L',  // linger on ExitToShell
	
	Opt_last_byte = 255,
	
	Opt_events_fd,
};

static command::option options[] =
{
	{ "linger",  Opt_linger },
	
	{ "events-fd", Opt_events_fd, command::Param_required },
	
	NULL,
};


void* SysEvtBuf : 0x0146;
QHdr EventQueue : 0x014A;
short SysEvtCnt : 0x0154;

short ScreenRow : 0x0106;
void* ScrnBase  : 0x0824;
Point Mouse     : 0x0830;
Rect  CrsrPin   : 0x0834;

void* os_trap_table     [] : 1 * 1024;
void* toolbox_trap_table[] : 3 * 1024;

#define OSTRAP( Proc )  (os_trap_table     [ _##Proc & 0x00FF ] = &Proc##_patch)
#define TBTRAP( Proc )  (toolbox_trap_table[ _##Proc & 0x03FF ] = &Proc##_patch)

enum
{
	_ReallocateHandle = _ReallocHandle,
	_ReserveMem       = _ResrvMem,
};


static inline
pascal asm void ScrnBitMap( BitMap* screenBits )
{
	DC.W     _ScrnBitMap
}

static void initialize_low_memory_globals()
{
	BitMap screenBits;
	
	ScrnBitMap( &screenBits );
	
	ScreenRow = screenBits.rowBytes;
	
	const short n_max_events = 20;
	
	const int event_size = 4 + sizeof (EvQEl);
	
	SysEvtBuf = calloc( event_size, n_max_events );
	SysEvtCnt = n_max_events - 1;
	
	ScrnBase = screenBits.baseAddr;
	
	*(long*) &Mouse = 0x000F000F;  // 15, 15
	
	CrsrPin = screenBits.bounds;
	
	init_lowmem_Cursor();
}

static void install_MemoryManager()
{
	OSTRAP( NewHandle        );  // A022
	OSTRAP( DisposeHandle    );  // A023
	OSTRAP( SetHandleSize    );  // A024
	OSTRAP( GetHandleSize    );  // A025
	OSTRAP( ReallocateHandle );  // A027
	OSTRAP( EmptyHandle      );  // A02B
	
	OSTRAP( SetApplLimit     );  // A02D
	
	OSTRAP( MoreMasters      );  // A036
	
	OSTRAP( ReserveMem       );  // A040
	OSTRAP( MaxApplZone      );  // A063
	OSTRAP( NewEmptyHandle   );  // A066
	
	TBTRAP( HandToHand  );  // A9E1
}

static void install_OSUtils()
{
	OSTRAP( Delay     );  // A03B
	TBTRAP( Enqueue   );  // A96E
	TBTRAP( Dequeue   );  // A96F
	TBTRAP( TickCount );  // A975
	TBTRAP( SysBeep   );  // A9C8
	TBTRAP( SysError  );  // A9C9
}

static void install_Gestalt()
{
	old_Gestalt = (Gestalt_ProcPtr) os_trap_table[ _Gestalt & 0x00FF ];
	
	OSTRAP( Gestalt );  // A1AD
}

static void install_FileManager()
{
	OSTRAP( Open   );  // A000
	
	OSTRAP( Create );  // A008
	
	OSTRAP( GetVol );  // A014
}

static void install_EventManager()
{
	OSTRAP( PostEvent    );  // A02F
	OSTRAP( OSEventAvail );  // A030
	OSTRAP( GetOSEvent   );  // A031
	OSTRAP( FlushEvents  );  // A032
	
	TBTRAP( WaitNextEvent );  // A860
	TBTRAP( GetNextEvent  );  // A970
	
	TBTRAP( GetMouse      );  // A972
	TBTRAP( StillDown     );  // A973
	TBTRAP( Button        );  // A974
}

static void install_DeskManager()
{
	TBTRAP( SystemTask );  // A9B4
}

static void install_SegmentLoader()
{
	TBTRAP( LoadSeg     );  // A9F0
	TBTRAP( Launch      );  // A9F2
	TBTRAP( ExitToShell );  // A9F4
}

static void install_Debugger()
{
	TBTRAP( Debugger );  // A9FF
	TBTRAP( DebugStr );  // ABFF
}

static asm void module_suspend()
{
	JSR      0xFFFFFFF8
}

static char* const* get_options( char** argv )
{
	using command::global_result;
	
	int opt;
	
	++argv;  // skip arg 0
	
	while ( (opt = command::get_option( (char* const**) &argv, options )) > 0 )
	{
		switch ( opt )
		{
			case Opt_linger:
				linger_on_exit = true;
				break;
			
			case Opt_events_fd:
				events_fd = gear::parse_unsigned_decimal( global_result.param );
				break;
			
			default:
				break;
		}
	}
	
	return argv;
}

int main( int argc, char** argv )
{
	if ( argc > 0 )
	{
		char* const* args = get_options( argv );
		
		if ( *args != NULL )
		{
			WARN( "no arguments allowed" );
			
			_exit( 1 );
		}
	}
	
	set_syserr_handler();
	
	initialize_low_memory_globals();
	
	install_MemoryManager();
	
	install_OSUtils();
	
	install_Gestalt();
	
	install_FileManager();
	
	install_EventManager();
	
	install_DeskManager();
	
	install_SegmentLoader();
	
	install_Debugger();
	
	module_suspend();  // doesn't return
}
