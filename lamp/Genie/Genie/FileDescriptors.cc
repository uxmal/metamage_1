/*	==================
 *	FileDescriptors.cc
 *	==================
 */

#include "Genie/FileDescriptors.hh"

// POSIX
#include "errno.h"

// POSeven
#include "POSeven/Errno.hh"

// Genie
#include "Genie/Process.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace P7 = POSeven;
	
	
	int LowestUnusedFileDescriptor( int fd )
	{
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		while ( files.find( fd ) != files.end() )
		{
			++fd;
		}
		
		return fd;
	}
	
	static FileDescriptorMap::iterator FindFDIterator( int fd )
	{
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		FileDescriptorMap::iterator it = files.find( fd );
		
		if ( it == files.end() )
		{
			P7::ThrowErrno( EBADF );
		}
		
		return it;
	}
	
	void CloseFileDescriptor( int fd )
	{
		FileDescriptorMap::iterator it = FindFDIterator( fd );
		
		CurrentProcess().FileDescriptors().erase( it );
	}
	
	void AssignFileDescriptor( int fd, const boost::shared_ptr< IOHandle >& handle )
	{
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		files[ fd ] = handle;
	}
	
	boost::shared_ptr< IOHandle > const& GetFileHandle( int fd )
	{
		FileDescriptorMap::const_iterator it = FindFDIterator( fd );
		
		return it->second.handle;
	}
	
}

