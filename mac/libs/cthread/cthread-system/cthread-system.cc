/*
	cthread-system.cc
	-----------------
*/

#include "cthread-system.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __THREADS__
#include <Threads.h>
#endif

// mac-sys-utils
#include "mac_sys/current_thread_stack_space.hh"
#include "mac_sys/gestalt.hh"
#include "mac_sys/init_thread.hh"

// cthread
#include "cthread/parameter_block.hh"
#include "cthread/types.hh"

// recall
#include "recall/stack_frame.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"


namespace cthread {
namespace system  {
	
	static
	void* measure_stack_limit()
	{
	#ifdef __MACOS__
		
		using recall::get_frame_pointer;
		using mac::sys::current_thread_stack_space;
		
		return (char*) get_frame_pointer() - current_thread_stack_space();
		
	#endif
		
		return NULL;
	}
	
	
	static
	pascal void* ThreadEntry( void* param )
	{
		parameter_block& pb = *(parameter_block*) param;
		
		pb.stack_bottom = mac::sys::init_thread();
		pb.stack_limit  = measure_stack_limit();
		
		try
		{
			return pb.start( pb.param, pb.stack_bottom, pb.stack_limit );
		}
		catch ( ... )
		{
		}
		
		return NULL;
	}
	
	static
	ThreadEntryTPP GetThreadEntryFunction()
	{
		static ThreadEntryTPP upp = NewThreadEntryUPP( &ThreadEntry );
		
		return upp;
	}
	
	
	static
	pascal void ThreadSwitchIn( ThreadID thread, void* param )
	{
		const parameter_block& pb = *(parameter_block*) param;
		
		if ( pb.switch_in )
		{
			pb.switch_in( pb.param );
		}
	}
	
	static
	pascal void ThreadSwitchOut( ThreadID thread, void* param )
	{
		const parameter_block& pb = *(parameter_block*) param;
		
		if ( pb.switch_out )
		{
			pb.switch_out( pb.param );
		}
	}
	
	static ThreadSwitchUPP switchIn  = NewThreadSwitchUPP( ThreadSwitchIn  );
	static ThreadSwitchUPP switchOut = NewThreadSwitchUPP( ThreadSwitchOut );
	
	thread_id create_thread( parameter_block& pb, unsigned stack_size )
	{
		::Size size = 0;
		
		// Jaguar returns paramErr
		OSStatus err = ::GetDefaultThreadStackSize( kCooperativeThread, &size );
		
		if ( size > stack_size )
		{
			stack_size = size;
		}
		
		thread_id thread;
		
		err = ::NewThread( kCooperativeThread,
		                   GetThreadEntryFunction(),
		                   &pb,
		                   stack_size,
		                   0,
		                   NULL,
		                   &thread );
		
		Mac::ThrowOSStatus( err );
		
		if ( pb.switch_in )
		{
			::SetThreadSwitcher( thread, switchIn, &pb, true );
		}
		
		if ( pb.switch_out )
		{
			::SetThreadSwitcher( thread, switchOut, &pb, false );
		}
		
		return thread;
	}
	
	void destroy_thread( thread_id id )
	{
		::DisposeThread( id, NULL, false );
	}
	
	thread_id current_thread()
	{
		::ThreadID thread;
		OSErr err = ::GetCurrentThread( &thread );
		
		return err != noErr ? kNoThreadID : thread;
	}
	
	unsigned long current_thread_stack_space()
	{
		return mac::sys::current_thread_stack_space();
	}
	
	bool is_thread_stopped( thread_id id )
	{
		::ThreadState state;
		
		OSErr err = ::GetThreadState( id, &state );
		
		Mac::ThrowOSStatus( err );
		
		return state == kStoppedThreadState;
	}
	
	void stop_thread( thread_id id )
	{
		OSErr err = ::SetThreadState( id, kStoppedThreadState, kNoThreadID );
		
		Mac::ThrowOSStatus( err );
	}
	
	void wake_thread( thread_id id )
	{
		OSErr err = ::SetThreadState( id, kReadyThreadState, kNoThreadID );
		
		Mac::ThrowOSStatus( err );
	}
	
	void thread_yield()
	{
		// Ignore errors so we don't throw in critical sections
		(void) ::YieldToAnyThread();
	}
	
}
}
