/*
	fetches.hh
	----------
*/

#ifndef V68K_FETCHES_HH
#define V68K_FETCHES_HH

// v68k
#include "v68k/fetcher.hh"


namespace v68k
{
	
	extern fetcher fetches_none[];
	
	extern fetcher fetches_data_at_0007[];
	
	extern fetcher fetches_MOVEP[];
	
	extern fetcher fetches_LINK[];
	
	extern fetcher fetches_STOP[];
	
	extern fetcher fetches_MOVEQ[];
	
	extern fetcher fetches_EXG[];
	
}

#endif

