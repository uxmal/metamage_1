// io/spew.hh
// ----------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef IO_SPEW_HH
#define IO_SPEW_HH

#ifndef IO_IO_HH
#include "io/io.hh"
#endif


namespace io
{
	
	class incomplete_spew {};
	
	template < class details >
	class spew_putter
	{
		private:
			typedef typename details::stream      stream;
			typedef typename details::byte_count  byte_count;
			
			stream its_output;
		
		public:
			spew_putter( const stream& output ) : its_output( output )
			{
			}
			
			void operator()( const void *begin, const void *end ) const
			{
				byte_count bytesToWrite = distance( begin, end );
				
				byte_count bytesRead = io::write< byte_count, stream >( its_output, (const char*) begin, bytesToWrite );
				
				if ( bytesRead != bytesToWrite )
				{
					throw incomplete_spew();
				}
			}
	};
	
	template < class Stream >
	void spew_output( const Stream& output, const char* buffer, std::size_t length )
	{
		spew_putter< io::iostream_traits< Stream > > putter( output );
		
		putter( buffer, buffer + length );
	}
	
	template < class Flattener, class Stream >
	void spew_output( const Stream& output, typename Flattener::argument_type param )
	{
		spew_putter< io::iostream_traits< Stream > > putter( output );
		
		Flattener::Put( param, putter );
	}
	
	template < class FileSpec >
	void spew_file( const FileSpec& file, const char* buffer, std::size_t length )
	{
		spew_output( open_truncated( file, overload() ).get(), buffer, length );
	}
	
	template < class Flattener, class FileSpec >
	void spew_file( const FileSpec& file, typename Flattener::argument_type param )
	{
		spew_output< Flattener >( open_truncated( file, overload() ).get(), param );
	}
	
}

#endif

