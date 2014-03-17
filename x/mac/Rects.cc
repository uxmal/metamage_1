/*
	Rects.cc
	--------
*/

#include "Rects.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// Standard C
#include <string.h>


struct rectangular_op_params
{
	Point topLeft;
	
	Ptr start;
	
	uint16_t height;
	
	uint16_t draw_bytes;
	uint16_t skip_bytes;
	
	uint8_t left_mask;
	uint8_t right_mask;
};

struct rectangular_fill_params : rectangular_op_params
{
	Pattern pattern;
};

static inline short min( short a, short b )
{
	return b < a ? b : a;
}

static inline short max( short a, short b )
{
	return a > b ? a : b;
}

const int delta_from_screenBits_to_thePort = offsetof(QDGlobals, thePort   )
                                           - offsetof(QDGlobals, screenBits);

static inline asm const BitMap* get_screenBits_() : __A0
{
	MOVEA.L  (A5),A0
	
	SUBA.L   delta_from_screenBits_to_thePort,A0
}

static inline const BitMap& get_screenBits()
{
	return *get_screenBits_();
}

static void do_Rect_intersection( Rect& result, const Rect& a, const Rect& b )
{
	result.top    = max( a.top,    b.top    );
	result.left   = max( a.left,   b.left   );
	result.bottom = min( a.bottom, b.bottom );
	result.right  = min( a.right,  b.right  );
}

static void get_rectangular_op_params_for_rect( rectangular_op_params&  params,
                                                const Rect&             input_rect )
{
	const BitMap& screenBits = get_screenBits();
	
	const Rect& bounds = screenBits.bounds;
	
	Rect rect;
	
	do_Rect_intersection( rect, bounds, input_rect );
	
	const int width_px  = rect.right - rect.left;
	const int height_px = rect.bottom - rect.top;
	
	if ( width_px <= 0  ||  height_px <= 0 )
	{
		return;
	}
	
	const unsigned top  = rect.top  - bounds.top;
	const unsigned left = rect.left - bounds.left;
	
	const unsigned right = left + width_px;
	
	const unsigned outer_left_bytes  =  left      / 8;
	const unsigned inner_left_bytes  = (left + 7) / 8;
	const unsigned inner_right_bytes =  right      / 8;
	const unsigned outer_right_bytes = (right + 7) / 8;
	
	const unsigned inner_bytes = inner_right_bytes - inner_left_bytes;
	const unsigned outer_bytes = outer_right_bytes - outer_left_bytes;
	
	const uint8_t left_mask  = (left  & 0x7) ? ~((1 << 8 - (left  & 0x7)) - 1) : 0;
	const uint8_t right_mask = (right & 0x7) ?   (1 << 8 - (right & 0x7)) - 1  : 0;
	
	const uint32_t rowBytes = screenBits.rowBytes;
	
	params.topLeft    = *(Point*) &rect;
	params.start      = screenBits.baseAddr + top * rowBytes + outer_left_bytes;
	params.height     = height_px;
	params.draw_bytes = inner_bytes;
	params.skip_bytes = rowBytes - outer_bytes;
	params.left_mask  = left_mask;
	params.right_mask = right_mask;
}

static void erase_rect( const rectangular_op_params& params )
{
	Ptr p = params.start;
	
	for ( int i = params.height;  i > 0;  --i )
	{
		if ( params.left_mask )
		{
			*p++ &= params.left_mask;
		}
		
		memset( p, '\0', params.draw_bytes );
		
		p += params.draw_bytes;
		
		if ( params.right_mask )
		{
			*p++ &= params.right_mask;
		}
		
		p += params.skip_bytes;
	}
}

pascal void EraseRect_patch( const Rect* rect )
{
	rectangular_op_params params;
	
	get_rectangular_op_params_for_rect( params, *rect );
	
	erase_rect( params );
}

static void paint_rect( const rectangular_op_params& params )
{
	Ptr p = params.start;
	
	for ( int i = params.height;  i > 0;  --i )
	{
		if ( params.left_mask )
		{
			*p++ |= ~params.left_mask;
		}
		
		memset( p, 0xFF, params.draw_bytes );
		
		p += params.draw_bytes;
		
		if ( params.right_mask )
		{
			*p++ |= ~params.right_mask;
		}
		
		p += params.skip_bytes;
	}
}

pascal void PaintRect_patch( const Rect* rect )
{
	rectangular_op_params params;
	
	get_rectangular_op_params_for_rect( params, *rect );
	
	paint_rect( params );
}

static void fill_rect( const rectangular_fill_params& params )
{
	Pattern pattern = params.pattern;
	
	short       v = params.topLeft.v % 8;
	short const h = params.topLeft.h % 8;
	
	if ( h != 0 )
	{
		for ( int i = 0;  i < 8;  ++i )
		{
			pattern.pat[i] = pattern.pat[i] <<      h
			               | pattern.pat[i] >> (8 - h);
		}
	}
	
	Ptr p = params.start;
	
	for ( int i = params.height;  i > 0;  --i )
	{
		const uint8_t pat = pattern.pat[v];
		
		if ( params.left_mask )
		{
			*p = (*p  &  params.left_mask)
			   | (pat & ~params.left_mask);
			
			++p;
		}
		
		memset( p, pat, params.draw_bytes );
		
		p += params.draw_bytes;
		
		if ( params.right_mask )
		{
			*p = (*p  &  params.right_mask)
			   | (pat & ~params.right_mask);
			
			++p;
		}
		
		p += params.skip_bytes;
		
		v = (v + 1) % 8;
	}
}

pascal void FillRect_patch( const Rect* rect, const Pattern* pattern )
{
	rectangular_fill_params params;
	
	get_rectangular_op_params_for_rect( params, *rect );
	
	params.pattern = *pattern;
	
	fill_rect( params );
}
