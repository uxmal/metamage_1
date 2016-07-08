/*
	array-utils.hh
	--------------
*/

#ifndef VLIB_ARRAYUTILS_HH
#define VLIB_ARRAYUTILS_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	Value linear_subscript( const Value& array, const Value& index );
	
	void push( const Value& array_target, const Value& list );
	
}

#endif
