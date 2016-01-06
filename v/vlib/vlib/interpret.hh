/*
	interpret.hh
	------------
*/

#ifndef VLIB_FUNCTIONS_HH
#define VLIB_FUNCTIONS_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	Value interpret( const char* program, const char* file = 0 );  // NULL
	
}

#endif
