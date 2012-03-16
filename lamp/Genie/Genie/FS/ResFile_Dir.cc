/*	==============
 *	ResFile_Dir.cc
 *	==============
 */

#include "Genie/FS/ResFile_Dir.hh"

// Standard C++
#include <algorithm>

// POSIX
#include <sys/stat.h>

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Files.hh"
#include "Nitrogen/Resources.hh"

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/dir_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/FS/resources.hh"
#include "Genie/Utilities/AsyncIO.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static void empty_rsrc_fork_mkdir( const FSTree* node, mode_t mode );
	
	static const dir_method_set empty_rsrc_fork_dir_methods =
	{
		NULL,
		NULL,
		&empty_rsrc_fork_mkdir
	};
	
	static const node_method_set empty_rsrc_fork_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&empty_rsrc_fork_dir_methods
	};
	
	class FSTree_EmptyRsrcForkDir : public FSTree
	{
		private:
			FSSpec  itsFileSpec;
		
		public:
			FSTree_EmptyRsrcForkDir( const FSTreePtr&     parent,
			                         const plus::string&  name,
			                         const FSSpec&        file )
			:
				FSTree( parent, name, 0, &empty_rsrc_fork_methods ),
				itsFileSpec( file )
			{
			}
			
			void CreateDirectory( mode_t mode ) const;
	};
	
	static void empty_rsrc_fork_mkdir( const FSTree* node, mode_t mode )
	{
		const FSTree_EmptyRsrcForkDir* file = static_cast< const FSTree_EmptyRsrcForkDir* >( node );
		
		file->CreateDirectory( mode );
	}
	
	
	static void resfile_dir_remove( const FSTree* node );
	
	static FSTreePtr resfile_dir_lookup( const FSTree*        node,
	                                     const plus::string&  name,
	                                     const FSTree*        parent );
	
	static void resfile_dir_listdir( const FSTree*  node,
	                                 FSTreeCache&   cache );
	
	static const dir_method_set resfile_dir_dir_methods =
	{
		&resfile_dir_lookup,
		&resfile_dir_listdir
	};
	
	static const node_method_set resfile_dir_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		&resfile_dir_remove,
		NULL,
		NULL,
		NULL,
		&resfile_dir_dir_methods
	};
	
	class FSTree_ResFileDir : public FSTree
	{
		private:
			FSSpec  itsFileSpec;
		
		public:
			FSTree_ResFileDir( const FSTreePtr&     parent,
			                   const plus::string&  name,
			                   const FSSpec&        file )
			:
				FSTree( parent, name, S_IFDIR | 0700, &resfile_dir_methods ),
				itsFileSpec( file )
			{
			}
			
			void Delete() const;
			
			FSTreePtr Lookup_Child( const plus::string& name, const FSTree* parent ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	static void resfile_dir_remove( const FSTree* node )
	{
		const FSTree_ResFileDir* file = static_cast< const FSTree_ResFileDir* >( node );
		
		file->Delete();
	}
	
	static FSTreePtr resfile_dir_lookup( const FSTree*        node,
	                                     const plus::string&  name,
	                                     const FSTree*        parent )
	{
		const FSTree_ResFileDir* file = static_cast< const FSTree_ResFileDir* >( node );
		
		return file->Lookup_Child( name, parent );
	}
	
	static void resfile_dir_listdir( const FSTree*  node,
	                                 FSTreeCache&   cache )
	{
		const FSTree_ResFileDir* file = static_cast< const FSTree_ResFileDir* >( node );
		
		file->IterateIntoCache( cache );
	}
	
	
	static bool ResFile_dir_exists( const FSSpec& file )
	{
		::ResFileRefNum refNum = ::FSpOpenResFile( &file, fsRdPerm );
		
		const bool exists = refNum >= 0;
		
		if ( exists )
		{
			::CloseResFile( refNum );
		}
		else
		{
			const OSErr err = ::ResError();
			
			if ( err != eofErr )
			{
				Mac::ThrowOSStatus( err );
			}
		}
		
		return exists;
	}
	
	static inline bool contains( const char* s, size_t length, char c )
	{
		const char* end = s + length;
		
		return std::find( s, end, c ) != end;
	}
	
	static inline bool is_rsrc_file( const CInfoPBRec&  cInfo,
	                                 ConstStr255Param   name )
	{
		return cInfo.hFileInfo.ioFlLgLen == 0  &&  !contains( (char*) &name[ 1 ],
		                                                      1 + name[0],
		                                                      '.' );
	}
	
	void FSTree_ResFileDir::Delete() const
	{
		n::owned< Mac::FSFileRefNum > resource_fork = N::FSpOpenRF( itsFileSpec, Mac::fsRdWrPerm );
		
		if ( N::GetEOF( resource_fork ) > 286 )
		{
			p7::throw_errno( ENOTEMPTY );
		}
		
		N::SetEOF( resource_fork, 0 );
	}
	
	void FSTree_EmptyRsrcForkDir::CreateDirectory( mode_t mode ) const
	{
		CInfoPBRec cInfo = { 0 };
		
		const bool exists = FSpGetCatInfo< FNF_Returns >( cInfo, false, itsFileSpec );
		
		::OSType creator;
		::OSType type;
		
		if ( !exists || is_rsrc_file( cInfo, itsFileSpec.name ) )
		{
			creator = 'RSED';
			type    = 'rsrc';
		}
		else
		{
			const FInfo& fInfo = cInfo.hFileInfo.ioFlFndrInfo;
			
			creator = fInfo.fdCreator;
			type    = fInfo.fdType;
		}
		
		N::FSpCreateResFile( itsFileSpec,
		                     Mac::FSCreator( creator ),
		                     Mac::FSType   ( type    ),
		                     Mac::smSystemScript );
	}
	
	FSTreePtr FSTree_ResFileDir::Lookup_Child( const plus::string& name, const FSTree* parent ) const
	{
		if ( !Exists() )
		{
			p7::throw_errno( ENOENT );
		}
		
		return Get_RsrcFile_FSTree( parent, name, itsFileSpec );
	}
	
	void FSTree_ResFileDir::IterateIntoCache( FSTreeCache& cache ) const
	{
		iterate_resources( itsFileSpec, cache );
	}
	
	
	FSTreePtr Get_ResFileDir_FSTree( const FSTreePtr&     parent,
	                                 const plus::string&  name,
	                                 const FSSpec&        file )
	{
		const bool exists = ResFile_dir_exists( file );
		
		typedef const FSTree* T;
		
		return exists ? T( new FSTree_ResFileDir      ( parent, name, file ) )
		              : T( new FSTree_EmptyRsrcForkDir( parent, name, file ) );
	}
	
}

