/*
	system-info.cc
	--------------
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

// Standard C
#include <stdio.h>

// gear
#include "gear/inscribe_decimal.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"
#include "mac_sys/get_machine_name.hh"
#include "mac_sys/unit_table.hh"


#define PROGRAM  "system-info"

#define STR_LEN( s )  "" s, (sizeof s - 1)


using mac::sys::gestalt;
using mac::sys::gestalt_defined;


#define M68K  "Motorola 68K"
#define PPC   "PowerPC"
#define X86   "x86"
#define WHAT  "???"

#define MOD_TYPE  "Execution module type:  "
#define COMPILED  "Compiled architecture:  "


struct SonyVars_record
{
	uint32_t zeros[ 3 ];
	uint32_t reserved;
	uint32_t magic;
	uint32_t address;
};

#if TARGET_CPU_68K

SonyVars_record* SonyVars : 0x0134;

#endif

static inline
bool in_supervisor_mode()
{
#ifdef __MC68K__
	
	uint16_t status;
	
	asm
	{
		MOVE.W   SR,status
	}
	
	return status & (1 << 13);
	
#endif
	
	return false;
}

static
void compiled()
{
	const char* type = TARGET_RT_MAC_MACHO ? MOD_TYPE "Mach-O"
	                 : TARGET_RT_MAC_CFM   ? MOD_TYPE "CFM"
	                 :                       MOD_TYPE "Code resource";
	
	printf( "%s\n", type );
	
	const char* arch = TARGET_CPU_68K ? COMPILED M68K
	                 : TARGET_CPU_PPC ? COMPILED PPC
	                 : TARGET_CPU_X86 ? COMPILED X86
	                 :                  COMPILED WHAT;
	
	printf( "%s\n", arch );
}

static
void host_env()
{
	const uint32_t sysa = gestalt( 'sysa' );
	const uint32_t sysv = gestalt( 'sysv' );
	
	const char* arch_name = sysa <=  1 ? M68K
	                      : sysa ==  2 ? PPC
	                      : sysa == 10 ? X86
	                      :              WHAT;
	
	char machine_name[ 256 ] = { 0 };
	
	const unsigned char* mnam = mac::sys::get_machine_name();
	
	if ( mnam != NULL )
	{
		memcpy( machine_name, mnam + 1, mnam[ 0 ] );
	}
	
	uint8_t sys1 = gestalt( 'sys1' );
	uint8_t sys2;
	uint8_t sys3;
	
	if ( sys1 )
	{
		sys2 = gestalt( 'sys2' );
		sys3 = gestalt( 'sys3' );
	}
	else
	{
		sys1 = (sysv >> 8) & 0xF;
		sys2 = (sysv >> 4) & 0xF;
		sys3 = (sysv >> 0) & 0xF;
	}
	
	char a[ 4 ] = { 0 };
	char b[ 4 ] = { 0 };
	char c[ 4 ] = { 0 };
	
	gear::inscribe_unsigned_decimal_r( sys1, a );
	gear::inscribe_unsigned_decimal_r( sys2, b );
	gear::inscribe_unsigned_decimal_r( sys3, c );
	
	const char* os_name = sysv < 0x0800 ? "Macintosh System"
	                    : sysv < 0x1000 ? "Mac OS"
	                    : sysv < 0x1080 ? "Mac OS X"
	                    : sys2 <     12 ? "OS X"
	                    :                 "macOS";
	
	printf( "Host CPU architecture:  %s\n", arch_name );
	
	if ( mnam != NULL )
	{
		printf( "Host CPU machine name:  %s\n", machine_name );
	}
	
	if ( sysv != 0 )
	{
		const char* o = ".";
		
		if ( sys3 == 0 )
		{
			++o;
			
			c[ 0 ] = '\0';
		}
		
		printf( "\n" );
		printf( "Host operating system:  %s %s.%s%s%s\n", os_name, a, b, o, c );
	}
	
	const uint32_t os = gestalt( 'os  ' );
	
	if ( os & (1 << 19) )
	{
		printf( "%s\n", "Application switching:  Process Manager (native)" );
	}
	else if ( os & (1 << 3) )
	{
		printf( "%s\n", "Application switching:  Process Manager" );
	}
	else if ( os & (1 << 1) )
	{
		printf( "%s\n", "Application switching:  MultiFinder" );
	}
	else
	{
		printf( "%s\n", "Application switching:  none" );
	}
	
	if ( ! TARGET_CPU_68K )
	{
		// Do nothing.  Thread Manager is implied and goes without saying.
	}
	else if ( gestalt( 'thds' ) )
	{
		printf( "%s\n", "Cooperative threading:  Thread Manager" );
	}
	else
	{
		printf( "%s\n", "Cooperative threading:  Bespoke threads" );
	}
	
	const uint32_t gestaltOpenTptTCPPresentMask = 0x00000010;
	
	if ( gestalt( 'otan' ) & gestaltOpenTptTCPPresentMask )
	{
		const uint16_t version = gestalt( 'otvr' ) >> 16;
		
		const int major = (version >> 8 & 0xF) + 10 * (version >> 12);
		const int minor =  version >> 4 & 0xF;
		const int patch =  version >> 0 & 0xF;
		
		const bool mach = TARGET_RT_MAC_MACHO;
		
		const char* text = mach ? "Open Transport TCP/IP:  "
		                        : "TCP/IP protocol stack:  Open Transport ";
		
		printf( "%s%d.%d.%d\n", text, major, minor, patch );
	}
	
	if ( TARGET_CPU_68K  &&  gestalt( 'mmu ' ) )
	{
		const int gestalt32BitAddressing = 0;
		
		const uint32_t addr = gestalt( 'addr' );
		
		const bool _32bits = addr & (1 << gestalt32BitAddressing);
		
		printf( "680x0 addressing mode:  %s-bit\n", _32bits ? "32" : "24" );
	}
	
	if ( TARGET_CPU_68K )
	{
		const char* level = in_supervisor_mode() ? "Supervisor" : "User";
		
		printf( "680x0 privilege level:  %s\n", level );
	}
	
	if ( ! TARGET_RT_MAC_MACHO  &&  gestalt( 'mmu ' ) > 1  &&  sysv < 0x1000 )
	{
		const char* status = gestalt( 'vm  ' ) ? "On" : "Off";
		
		printf( "Virtual memory status:  %s\n", status );
	}
	
	if ( !! TARGET_RT_MAC_CFM  &&  TARGET_API_MAC_CARBON )
	{
		const uint32_t cbon = gestalt( 'cbon' );
		
		const char a = 0x30 + ((cbon >> 8) & 0xF);
		const char b = 0x30 + ((cbon >> 4) & 0xF);
		const char c = 0x30 + ((cbon >> 0) & 0xF);
		
		printf( "CarbonLib version:      %c.%c.%c\n", a, b, c );
	}
}

static
bool in_MinivMac()
{
#if TARGET_CPU_68K
	
	if ( SonyVars  &&  ((uint32_t) SonyVars & 1) == 0 )
	{
		const uint32_t magic = 0x841339E2;
		
		const uint32_t* p = SonyVars->zeros;
		
		uint32_t zero = 0;
		
		zero |= *p++;
		zero |= *p++;
		zero |= *p++;
		
		return zero == 0  &&  SonyVars->magic == magic;
	}
	
#endif
	
	return false;
}

static
bool in_SheepShaver()
{
	using mac::types::AuxDCE;
	
	if ( TARGET_RT_MAC_MACHO )
	{
		return false;
	}
	
	const short n = mac::sys::get_unit_table_entry_count();
	
	AuxDCE*** const begin = mac::sys::get_unit_table_base();
	AuxDCE*** const end   = begin + n;
	
	for ( AuxDCE*** it = begin;  it < end;  ++it )
	{
		const unsigned char* name = mac::sys::get_driver_name( *it );
		
		if ( name[ 0 ] == 0 )
		{
			continue;
		}
		
		const bool undotted = name[ 1 ] != '.';
		
		const int cmp = memcmp( &".Display_Video_Apple_Sheep"[ undotted ],
		                        name + 1,
		                        name[ 0 ] );
		
		if ( cmp == 0 )
		{
			return true;
		}
		
		++it;
	}
	
	return false;
}

static
void virt_env()
{
	const char* blank = "\n";
	
	if ( gestalt( 'a/ux' ) )
	{
		printf( "%s" "Paravirtualization:     A/UX\n", blank );
		blank = "";
	}
	
	const bool bbox = gestalt( 'bbox' );
	
	if ( bbox )
	{
		printf( "%s" "Paravirtualization:     Blue Box\n", blank );
		blank = "";
	}
	
	if ( TARGET_CPU_68K  &&  gestalt( 'sysa' ) == 2 )
	{
		printf( "%s" "68K emulation:          Apple\n", blank );
		blank = "";
	}
	
	if ( TARGET_CPU_68K  &&  gestalt_defined( 'v68k' ) )
	{
		printf( "%s" "68K emulation:          v68k\n", blank );
		blank = "";
	}
	else if ( TARGET_CPU_68K  &&  in_MinivMac() )
	{
		printf( "%s" "68K emulation:          Mini vMac\n", blank );
		blank = "";
	}
	else if ( in_SheepShaver() )
	{
		printf( "%s" "PPC emulation:          SheepShaver\n", blank );
		blank = "";
	}
	
	if ( !! TARGET_CPU_PPC  &&  TARGET_API_MAC_CARBON  &&  ! bbox )
	{
		const unsigned char* name = mac::sys::get_machine_name();
		
		const bool powerpc = name != NULL  &&  name[ 1 ] == 'P';
		
		if ( ! powerpc )
		{
			printf( "%s" "PPC emulation:          Rosetta\n", blank );
			blank = "";
		}
	}
}

int main( int argc, char** argv )
{
	compiled();
	host_env();
	virt_env();
	
	return 0;
}
