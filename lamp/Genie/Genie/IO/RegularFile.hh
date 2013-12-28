/*	==============
 *	RegularFile.hh
 *	==============
 */

#ifndef GENIE_IO_REGULARFILE_HH
#define GENIE_IO_REGULARFILE_HH

 // Genie
 #include "Genie/IO/Stream.hh"


namespace Genie
{
	
	class RegularFileHandle : public StreamHandle
	{
		public:
			RegularFileHandle( int                                flags,
			                   const vfs::filehandle_method_set*  methods = NULL );
			
			RegularFileHandle( const vfs::node_ptr&               file,
			                   int                                flags,
			                   const vfs::filehandle_method_set*  methods = NULL );
			
			virtual ~RegularFileHandle();
			
			memory_mapping_ptr Map( size_t length, int prot, int flags, off_t offset );
	};
	
}

#endif

