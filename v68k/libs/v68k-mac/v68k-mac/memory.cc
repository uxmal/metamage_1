/*
	memory.cc
	---------
*/

#include "v68k-mac/memory.hh"

// Standard C
#include <string.h>

// v68k
#include "v68k/endian.hh"

// v68k-mac
#include "v68k-mac/dynamic_globals.hh"


#pragma exceptions off


namespace v68k {
namespace mac  {

enum
{
	tag_ScreenRow,
	tag_SysEvtMask,
	tag_SysEvtBuf,
	tag_SysEvtBuf_low_word,
	tag_EventQueue,
	tag_EventQueue_word_4 = tag_EventQueue + 4,  // 10 bytes
	tag_EvtBufCnt,
	tag_Ticks,
	tag_Ticks_low_word,
	tag_MBState_etc,
	tag_KeyMods,
	tag_MemErr,
	tag_JHideCursor,
	tag_JHideCursor_low_word,
	tag_JShowCursor,
	tag_JShowCursor_low_word,
	tag_JInitCrsr,
	tag_JInitCrsr_low_word,
	tag_JSetCrsr,
	tag_JSetCrsr_low_word,
	tag_ScrnBase,
	tag_ScrnBase_low_word,
	tag_Mouse,
	tag_Mouse_low_word,
	tag_CrsrPin,
	tag_CrsrPin_word_3 = tag_CrsrPin + 3,  // 8 bytes
	tag_CurStackBase,
	tag_CurStackBase_low_word,
	tag_CurApName,
	tag_CurApName_word_15 = tag_CurApName + 15,  // 32 bytes
	tag_CurJTOffset,
	tag_ROMFont0,
	tag_ROMFont0_low_word,
	tag_WindowList,
	tag_WindowList_low_word,
	tag_SaveUpdate,
	tag_PaintWhite,
	tag_WMgrPort,
	tag_WMgrPort_low_word,
	tag_OldStructure,
	tag_OldStructure_low_word,
	tag_OldContent,
	tag_OldContent_low_word,
	tag_GrayRgn,
	tag_GrayRgn_low_word,
	tag_SaveVisRgn,
	tag_SaveVisRgn_low_word,
	tag_IconBitmap,
	tag_IconBitmap_word_6 = tag_IconBitmap + 6,  // 14 bytes
	tag_DragPattern,
	tag_DragPattern_word_3 = tag_DragPattern + 3,  // 8 bytes
	tag_DeskPattern,
	tag_DeskPattern_word_3 = tag_DeskPattern + 3,  // 8 bytes
	tag_ResErr,
	tag_CurActivate,
	tag_CurActivate_low_word,
	tag_CurDeactive,
	tag_CurDeactive_low_word,
	tag_MBarHeight,
	tag_last_A_trap,
	n_words
};

static uint16_t words[ n_words ];

struct global
{
	uint16_t  addr;
	uint8_t   size_;
	uint8_t   index;
	
	uint8_t size() const  { return size_ & 0x3F; }
	
	uint16_t word() const  { return int16_t( int8_t( index ) ); }
};

static inline bool operator<( const global& g, uint16_t addr )
{
	return g.addr + g.size() <= addr;
}

static const global globals[] =
{
	{ 0x0102, 0x84, 72              },  // ScrVRes, ScrHRes
	{ 0x0106, 2,    tag_ScreenRow   },
	{ 0x0144, 2,    tag_SysEvtMask  },
	{ 0x0146, 4,    tag_SysEvtBuf   },
	{ 0x014A, 10,   tag_EventQueue  },
	{ 0x0154, 2,    tag_EvtBufCnt   },
	{ 0x016A, 0x44, tag_Ticks       },
	{ 0x0172, 2,    tag_MBState_etc },  // MBState, Tocks
	{ 0x017A, 2,    tag_KeyMods     },  // (undocumented)
	{ 0x0220, 2,    tag_MemErr      },
	{ 0x031A, 0x83, 0xFF            },  // Lo3Bytes
	{ 0x0800, 4,    tag_JHideCursor },
	{ 0x0804, 4,    tag_JShowCursor },
	{ 0x0814, 4,    tag_JInitCrsr   },
	{ 0x0818, 4,    tag_JSetCrsr    },
	{ 0x0824, 4,    tag_ScrnBase    },
	{ 0x0830, 4,    tag_Mouse       },
	{ 0x0834, 8,    tag_CrsrPin     },
	{ 0x0908, 4,    tag_CurStackBase},
	{ 0x0910, 32,   tag_CurApName   },
	{ 0x0934, 2,    tag_CurJTOffset },
	{ 0x0980, 4,    tag_ROMFont0    },
	{ 0x0984, 0x82, 3               },  // ApFontID (Geneva)
	{ 0x09D6, 4,    tag_WindowList  },
	{ 0x09DA, 2,    tag_SaveUpdate  },
	{ 0x09DC, 2,    tag_PaintWhite  },
	{ 0x09DE, 4,    tag_WMgrPort    },
	{ 0x09E6, 4,    tag_OldStructure},
	{ 0x09EA, 4,    tag_OldContent  },
	{ 0x09EE, 4,    tag_GrayRgn     },
	{ 0x09F2, 4,    tag_SaveVisRgn  },
	{ 0x0A02, 0x84, 0x01            },  // OneOne
	{ 0x0A06, 0x84, 0xFF            },  // MinusOne
	{ 0x0A0E, 14,   tag_IconBitmap  },
	{ 0x0A34, 8,    tag_DragPattern },
	{ 0x0A3C, 8,    tag_DeskPattern },
	{ 0x0A60, 2,    tag_ResErr      },
	{ 0x0A64, 4,    tag_CurActivate },
	{ 0x0A68, 4,    tag_CurDeactive },
	{ 0x0BAA, 2,    tag_MBarHeight  },
	{ 0x0BFE, 2,    tag_last_A_trap }
};

static
const global* find( const global* begin, const global* end, uint16_t address )
{
	while ( begin < end )
	{
		if ( ! (*begin < address) )
		{
			break;
		}
		
		++begin;
	}
	
	return begin;
}

static const global* find_global( uint16_t address )
{
	const global* begin = globals;
	const global* end   = globals + sizeof globals / sizeof globals[0];
	
	const global* it = find( begin, end, address );
	
	if ( it == end )
	{
		return NULL;
	}
	
	if ( address < it->addr )
	{
		return NULL;
	}
	
	return it;
}

static void refresh_dynamic_global( uint8_t tag )
{
	uint16_t* address = &words[ tag ];
	
	uint32_t longword;
	
	switch ( tag )
	{
		case tag_Ticks:
			longword = get_Ticks();
			
			*(uint32_t*) address = big_longword( longword );
			
			break;
		
		default:
			break;
	}
}

static uint8_t buffer[ 16 ];

static uint8_t* read_globals( const global* g, uint32_t addr, uint32_t size )
{
	// size == 1 -> offset = 0
	// size == 2 -> offset = addr & 1
	// size == 4 -> offset = addr & 3
	
	const int offset = addr & size - 1;
	
	int i = offset - (addr - g->addr);
	
	const uint32_t end = addr + size;
	
	for ( ;; )
	{
		uint32_t width = g->size();
		
		if ( int8_t( g->size_ ) < 0 )
		{
			if ( width < 2 )
			{
				buffer[ i++ ] = g->index;
			}
			else
			{
				const uint16_t word = g->word();
				
				buffer[ i++ ] = word >> 8;
				buffer[ i++ ] = word;
				
				if ( width > 2 )
				{
					buffer[ i++ ] = word >> 8;
					buffer[ i++ ] = word;
					
					if ( width == 3 )
					{
						buffer[ i - 4 ] = 0;
						
						width = 4;
					}
				}
			}
		}
		else
		{
			if ( g->size_ >= 0x40 )
			{
				refresh_dynamic_global( g->index );
			}
			
			return (uint8_t*) &words[ g->index ] + (addr - g->addr);
		}
		
		addr += width;
		
		if ( addr >= end )
		{
			return buffer + offset;
		}
		
		if ( ++g == (globals + sizeof globals / sizeof globals[0]) )
		{
			return NULL;
		}
		
		if ( g->addr != addr )
		{
			return NULL;
		}
	}
	
	return buffer + offset;
}

static uint8_t* write_globals( const global* g, uint32_t addr, uint32_t size )
{
	const uint32_t offset = addr - g->addr;
	
	if ( offset + size <= g->size_ )
	{
		return buffer + offset;
	}
	
	return NULL;
}

static uint8_t* update_globals( const global* g, uint32_t addr, uint32_t size )
{
	const uint32_t offset = addr - g->addr;
	
	memcpy( (char*) &words[ g->index ] + offset, buffer + offset, size );
	
	return buffer;
}

uint8_t* low_memory::translate( uint32_t               addr,
                                uint32_t               length,
                                v68k::function_code_t  fc,
                                v68k::memory_access_t  access ) const
{
	if ( access == mem_exec )
	{
		// None of Mac low memory is executable
		
		return 0;  // NULL
	}
	
	if ( const global* g = find_global( addr ) )
	{
		if ( access == mem_read )
		{
			return read_globals( g, addr, length );
		}
		else if ( access == mem_write )
		{
			return write_globals( g, addr, length );
		}
		else  // mem_update
		{
			return update_globals( g, addr, length );
		}
	}
	
	return 0;
}

}  // namespace mac
}  // namespace v68k
