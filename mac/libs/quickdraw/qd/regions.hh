/*
	regions.hh
	----------
*/

#ifndef QUICKDRAW_REGIONS_HH
#define QUICKDRAW_REGIONS_HH

// #include <MacTypes.h>
struct Rect;


namespace quickdraw
{
	
	struct region_geometry_t
	{
		short n_v_coords;
		short n_h_coords;  // max number of horizontal coordinates
	};
	
	region_geometry_t region_geometry( const short* extent );
	
	void set_region_bbox( Rect& bbox, const short* extent );
	
}

#endif
