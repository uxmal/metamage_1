/*
	cthread-either.hh
	-----------------
*/

#ifndef CTHREADEITHER_HH
#define CTHREADEITHER_HH

#if ! defined( __MACOS__ )  ||  ! defined( __MC68K__ )
#define CTHREAD_SYSTEM  1
#undef  CTHREAD_CUSTOM
#elif CTHREAD_CUSTOM
#undef  CTHREAD_SYSTEM
#else
#define CTHREAD_EITHER  1
#endif

#if CTHREAD_SYSTEM
// cthread-system
#include "cthread-system.hh"
#endif

#if CTHREAD_CUSTOM
// cthread-custom
#include "cthread-custom.hh"
#endif


namespace cthread
{
	
	struct parameter_block;
	
	typedef unsigned long thread_id;
	
#if CTHREAD_SYSTEM
	
	namespace either = system;
	
#endif
	
#if CTHREAD_CUSTOM
	
	namespace either = custom;
	
#endif
	
#if CTHREAD_EITHER

namespace either
{
	
	using cthread::parameter_block;
	
	void thread_yield();
	
	unsigned long current_thread_stack_space();
	
	thread_id current_thread();
	
	thread_id create_thread( parameter_block& pb, unsigned stack_size );
	
	void destroy_thread( thread_id id );
	
	bool is_thread_stopped( thread_id id );
	
	void stop_thread( thread_id id );
	void wake_thread( thread_id id );
	
}

#endif  // #if CTHREAD_EITHER

}

#endif
