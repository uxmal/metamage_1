/*	========
 *	Abort.cc
 *	========
 */

// Mac OS
#include <Processes.h>

// Standard C
#include <signal.h>
#include <stdlib.h>

// Genie
#include "Genie/Process.hh"


void abort()
{
	try
	{
		Genie::DeliverFatalSignal( SIGABRT );
	}
	catch ( ... )
	{
	}
	
	::ExitToShell();
}
	
