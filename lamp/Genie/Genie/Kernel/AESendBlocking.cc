/*	=================
 *	AESendBlocking.cc
 *	=================
 */

// Relix
#include "relix/AESendBlocking.h"
#include "relix/syscall/registry.hh"

// Nitrogen
#include "Mac/Toolbox/Types/OSStatus.hh"

#include "Nitrogen/AEDataModel.hh"
#include "Nitrogen/AEInteraction.hh"

// Nitrogen Extras / AEFramework
#include "AEFramework/AEFramework.h"

// relix
#include "relix/api/current_thread.hh"
#include "relix/signal/signal.hh"
#include "relix/task/scheduler.hh"
#include "relix/task/thread.hh"

// Genie
#include "Genie/api/signals.hh"


namespace N = Nitrogen;


OSStatus AESendBlocking( const AppleEvent* appleEventPtr, AppleEvent* replyPtr )
{
	using namespace relix;
	
	try
	{
		Mac::AppleEvent const& appleEvent = static_cast< const Mac::AppleEvent& >( *appleEventPtr );
		Mac::AppleEvent      & reply      = static_cast<       Mac::AppleEvent& >( *replyPtr      );
		
		reply.dataHandle = NULL;
		
		(void) N::AESend( appleEvent,
		                  Mac::kAEQueueReply | Mac::kAECanInteract );
		
		// Now that we've sent the event, retrieve the return ID
		N::AEReturnID_32Bit returnID = N::AEGetAttributePtr< Mac::keyReturnIDAttr >( appleEvent );
		
		// Subscribe to AEFramework's queued reply delivery and wake-up service
		N::ExpectReply( returnID, &reply );
		
		const thread& current = current_thread();
		const pid_t   tid     = current.id();
		
	stop_and_wait:
		
		relix::mark_thread_inactive( tid );
		
		// Sleep until the reply is delivered
		stop_os_thread( current.get_os_thread() );
		
		relix::mark_thread_active( tid );
		
		/*
			Check for fatal signals; ignore caught ones (for now).
			
			TODO:  Return errAEWaitCanceled for a caught signal, and check
			on entry to determine if the Apple event has already been sent by
			a prior entry to this call.  We could possibly use keyReturnIDAttr
			for this, as long as the caller doesn't provide us with an event
			that's been sent previously.
		*/
		
		check_signals( false );  // Don't throw caught signals
		
		if ( reply.dataHandle == NULL )
		{
			goto stop_and_wait;
		}
	}
	catch ( const relix::signal& sig )
	{
		relix::the_signal = sig;
		
		// Only a fatal signal gets here, so actual error code doesn't matter
		return errAEWaitCanceled;
	}
	catch ( const Mac::OSStatus& err )
	{
		return err;
	}
	
	return noErr;
}

#pragma force_active on

REGISTER_SYSTEM_CALL( AESendBlocking );

#pragma force_active reset
