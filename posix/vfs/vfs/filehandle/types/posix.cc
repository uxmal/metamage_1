/*
	posix.cc
	--------
*/

#include "vfs/filehandle/types/posix.hh"

// poseven
#include "poseven/functions/fstat.hh"
#include "poseven/functions/pread.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/methods/bstore_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	struct posix_file_extra
	{
		int fd;
	};
	
	static ssize_t posix_pread( filehandle* file, char* buffer, size_t n, off_t offset )
	{
		posix_file_extra& extra = *(posix_file_extra*) file->extra();
		
		const p7::fd_t fd = p7::fd_t( extra.fd );
		
		return p7::pread( fd, buffer, n, offset );
	}
	
	static off_t posix_geteof( filehandle* file )
	{
		posix_file_extra& extra = *(posix_file_extra*) file->extra();
		
		const p7::fd_t fd = p7::fd_t( extra.fd );
		
		struct stat sb = p7::fstat( fd );
		
		return sb.st_size;
	}
	
	static const bstore_method_set posix_bstore_methods =
	{
		&posix_pread,
		&posix_geteof,
	};
	
	static const filehandle_method_set posix_methods =
	{
		&posix_bstore_methods,
	};
	
	static void close_posix_file( filehandle* that )
	{
		posix_file_extra& extra = *(posix_file_extra*) that->extra();
		
		::close( extra.fd );
	}
	
	filehandle_ptr new_posix_fd( int flags, int fd )
	{
		filehandle* result = new filehandle( flags,
		                                     &posix_methods,
		                                     sizeof (posix_file_extra),
		                                     &close_posix_file );
		
		posix_file_extra& extra = *(posix_file_extra*) result->extra();
		
		extra.fd = fd;
		
		return result;
	}
	
}
