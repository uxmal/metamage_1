/*
	gui/new/caption.cc
	------------------
*/

#include "Genie/FS/gui/new/caption.hh"

// Standard C++
#include <algorithm>

// POSIX
#include <fcntl.h>
#include <sys/stat.h>

// plus
#include "plus/mac_utf8.hh"
#include "plus/serialize.hh"
#include "plus/simple_map.hh"
#include "plus/var_string.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/methods/bstore_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/primitives/get_file.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/node/types/property_file.hh"

// Pedestal
#include "Pedestal/Caption.hh"

// Genie
#include "Genie/FS/Views.hh"


namespace Genie
{
	
	namespace Ped = Pedestal;
	
	
	struct CaptionParameters
	{
		plus::var_string  its_mac_text;
		plus::var_string  its_utf8_text;
		bool              itIsWrapped;
		bool              disabling;
		
		CaptionParameters() : itIsWrapped( true ), disabling()
		{
		}
	};
	
	typedef plus::simple_map< const vfs::node*, CaptionParameters > CaptionParametersMap;
	
	static CaptionParametersMap gCaptionParametersMap;
	
	
	class Caption : public Ped::Caption
	{
		private:
			typedef const vfs::node* Key;
			
			Key itsKey;
			bool  itIsActive;
		
		public:
			Caption( Key key ) : itsKey( key ), itIsActive( true )
			{
			}
			
			plus::string Text() const;
			
			bool Wrapped() const;
			bool Disabled() const;
			
			void Activate( bool activating )  { itIsActive = activating; }
	};
	
	plus::string Caption::Text() const
	{
		plus::var_string result;
		
		if ( CaptionParameters* it = gCaptionParametersMap.find( itsKey ) )
		{
			result = it->its_mac_text;
		}
		
		std::replace( result.begin(), result.end(), '\n', '\r' );
		
		return result;
	}
	
	bool Caption::Wrapped() const
	{
		if ( CaptionParameters* it = gCaptionParametersMap.find( itsKey ) )
		{
			return it->itIsWrapped;
		}
		
		return true;
	}
	
	bool Caption::Disabled() const
	{
		if ( CaptionParameters* it = gCaptionParametersMap.find( itsKey ) )
		{
			return !itIsActive && it->disabling;
		}
		
		return false;
	}
	
	static boost::intrusive_ptr< Ped::View > CreateView( const vfs::node* delegate )
	{
		return new Caption( delegate );
	}
	
	
	static void DestroyDelegate( const vfs::node* delegate )
	{
		gCaptionParametersMap.erase( delegate );
	}
	
	
	static void CaptionText_SetEOF( const vfs::node* text, off_t length )
	{
		const vfs::node* view = text->owner();
		
		CaptionParameters& params = gCaptionParametersMap[ view ];
		
		params.its_utf8_text.resize( length );
		
		params.its_mac_text = plus::mac_from_utf8( params.its_utf8_text );
		
		InvalidateWindowForView( view );
	}
	
	class CaptionTextFileHandle : public vfs::filehandle
	{
		public:
			CaptionTextFileHandle( const vfs::node& file, int flags );
			
			const vfs::node* ViewKey();
			
			plus::var_string& String()  { return gCaptionParametersMap[ ViewKey() ].its_utf8_text; }
			
			ssize_t Positioned_Read( char* buffer, size_t n_bytes, off_t offset );
			
			ssize_t Positioned_Write( const char* buffer, size_t n_bytes, off_t offset );
			
			off_t GetEOF()  { return String().size(); }
			
			void SetEOF( off_t length )  { CaptionText_SetEOF( get_file( *this ).get(), length ); }
	};
	
	
	static ssize_t caption_text_pread( vfs::filehandle* file, char* buffer, size_t n, off_t offset )
	{
		return static_cast< CaptionTextFileHandle& >( *file ).Positioned_Read( buffer, n, offset );
	}
	
	static off_t caption_text_geteof( vfs::filehandle* file )
	{
		return static_cast< CaptionTextFileHandle& >( *file ).GetEOF();
	}
	
	static ssize_t caption_text_pwrite( vfs::filehandle* file, const char* buffer, size_t n, off_t offset )
	{
		return static_cast< CaptionTextFileHandle& >( *file ).Positioned_Write( buffer, n, offset );
	}
	
	static void caption_text_seteof( vfs::filehandle* file, off_t length )
	{
		static_cast< CaptionTextFileHandle& >( *file ).SetEOF( length );
	}
	
	static const vfs::bstore_method_set caption_text_bstore_methods =
	{
		&caption_text_pread,
		&caption_text_geteof,
		&caption_text_pwrite,
		&caption_text_seteof,
	};
	
	static const vfs::filehandle_method_set caption_text_filehandle_methods =
	{
		&caption_text_bstore_methods,
	};
	
	
	CaptionTextFileHandle::CaptionTextFileHandle( const vfs::node& file, int flags )
	:
		vfs::filehandle( &file, flags, &caption_text_filehandle_methods )
	{
	}
	
	const vfs::node* CaptionTextFileHandle::ViewKey()
	{
		return get_file( *this )->owner();
	}
	
	ssize_t CaptionTextFileHandle::Positioned_Read( char* buffer, size_t n_bytes, off_t offset )
	{
		const plus::string& s = String();
		
		if ( offset >= s.size() )
		{
			return 0;
		}
		
		n_bytes = std::min< size_t >( n_bytes, s.size() - offset );
		
		memcpy( buffer, &s[ offset ], n_bytes );
		
		return n_bytes;
	}
	
	ssize_t CaptionTextFileHandle::Positioned_Write( const char* buffer, size_t n_bytes, off_t offset )
	{
		const vfs::node* view = ViewKey();
		
		CaptionParameters& params = gCaptionParametersMap[ view ];
		
		plus::var_string& s = params.its_utf8_text;
		
		if ( offset + n_bytes > s.size() )
		{
			s.resize( offset + n_bytes );
		}
		
		std::copy( buffer,
		           buffer + n_bytes,
		           s.begin() + offset );
		
		params.its_mac_text = plus::mac_from_utf8( s );
		
		InvalidateWindowForView( view );
		
		return n_bytes;
	}
	
	
	static void caption_text_seteof( const vfs::node* that, off_t length )
	{
		CaptionText_SetEOF( that, length );
	}
	
	static off_t caption_text_geteof( const vfs::node* that )
	{
		return gCaptionParametersMap[ that->owner() ].its_utf8_text.size();
	}
	
	static vfs::filehandle_ptr caption_text_open( const vfs::node* that, int flags, mode_t mode )
	{
		return new CaptionTextFileHandle( *that, flags );
	}
	
	static const vfs::data_method_set caption_text_data_methods =
	{
		&caption_text_open,
		&caption_text_geteof,
		&caption_text_seteof
	};
	
	static const vfs::node_method_set caption_text_methods =
	{
		NULL,
		&caption_text_data_methods
	};
	
	static vfs::node_ptr caption_text_factory( const vfs::node*     parent,
	                                           const plus::string&  name,
	                                           const void*          args )
	{
		return new vfs::node( parent, name, S_IFREG | 0600, &caption_text_methods );
	}
	
	
	namespace
	{
		
		bool& Wrapped( const vfs::node* view )
		{
			return gCaptionParametersMap[ view ].itIsWrapped;
		}
		
		bool& Disabling( const vfs::node* view )
		{
			return gCaptionParametersMap[ view ].disabling;
		}
		
	}
	
	typedef View_Property< plus::serialize_bool, Wrapped   > Wrapped_Property;
	typedef View_Property< plus::serialize_bool, Disabling > Disabling_Property;
	
	#define PROPERTY( prop )  &vfs::new_property, &vfs::property_params_factory< prop >::value
	
	static const vfs::fixed_mapping local_mappings[] =
	{
		{ "text", &caption_text_factory },
		
		{ "wrapped",   PROPERTY( Wrapped_Property   ) },
		{ "disabling", PROPERTY( Disabling_Property ) },
		
		{ NULL, NULL }
	};
	
	vfs::node_ptr New_FSTree_new_caption( const vfs::node*    parent,
	                                      const plus::string&  name,
	                                      const void*          args )
	{
		return New_new_view( parent,
		                     name,
		                     &CreateView,
		                     local_mappings,
		                     &DestroyDelegate );
	}
	
}
