// nucleus/scribe.hh
// -----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2007 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NUCLEUS_SCRIBE_HH
#define NUCLEUS_SCRIBE_HH

/*
	Some interfaces, like AppleEvent descriptors and Carbon Event parameters,
	move data around in a way that is untyped from a C++ perspective, instead
	treating type information as run-time data.
	
	The Nucleus scheme for building type-safe interfaces on top of these
	type-unsafe interfaces has three parts: flatteners, getters, and putters.
	
	Getters and putters are responsible for the mechanics of moving bytes,
	but not for relating the bytes to C++ types.  Getters and putters put
	uniform faces on functions that produce or consume untyped data, like
	AEGetParamPtr or SetDragItemFlavorData.
	
	A putter is just a functor that operates on a byte range:
	
	class Putter
	{
		void operator()( const void *begin, const void *end ) const;
	};
	
	And a getter is roughly the opposite: it fills in a byte range.
	It can also measure the number of bytes available.
	
	class Getter
	{
		std::size_t size() const;
		void operator()( void *begin, void *end ) const;
	};
	
	Scribes are responsible for rendering typed data as a sequence of bytes and
	rendering sequences of bytes into typed data.  They use getters and putters
	to move the raw bytes.
	
	class scribe
	{
		typedef P param_type;
		
		template < class Putter > static void Put( param_type, Putter );
		
		typedef R result_type;
		
		template < class Getter > static result_type Get( Getter );
	};
*/


namespace nucleus
{
	
	template < class T >
	struct POD_scribe
	{
		typedef const T&  argument_type;
		typedef       T   result_type;
		
		template < class Putter >
		static void Put( argument_type toPut, const Putter& put )
		{
			put( &toPut, &toPut + 1 );
		}
		
		template < class Getter >
		static result_type Get( const Getter& get )
		{
			result_type result;
			
			get( &result, &result + 1 );
			return result;
		}
		
		static const bool hasStaticSize = true;
		typedef T Buffer;
	};
	
	template < class T >
	struct POD_vector_scribe
	{
		typedef const T&  argument_type;
		typedef       T   result_type;
		
		template < class Putter >
		static void Put( argument_type toPut, const Putter& put )
		{
			put( &*toPut.begin(), &*toPut.end() );
		}
		
		template < class Getter >
		static result_type Get( const Getter& get )
		{
			result_type result;
			
			result.resize( get.size() );
			
			get( &*result.begin(), &*result.end() );
			return result;
		}
		
		static const bool hasStaticSize = false;
		struct Buffer {};
	};
	
	template < class Converted, class BaseFlattener >
	struct converting_scribe
	{
		typedef const Converted&  argument_type;
		typedef       Converted   result_type;
		
		template < class Putter >
		static void Put( argument_type toPut, const Putter& put )
		{
			BaseFlattener::Put( toPut, put );
		}
		
		template < class Getter >
		static result_type Get( const Getter& get )
		{
			return result_type( BaseFlattener::Get( get ) );
		}
		
		static const bool hasStaticSize = BaseFlattener::hasStaticSize;
		
		typedef typename BaseFlattener::Buffer Buffer;
	};
	
	template < class Converted, class POD >
	struct converting_POD_scribe : public converting_scribe< Converted, POD_scribe< POD > >
	{
	};
	
	
	struct empty {};
	
	struct empty_scribe
	{
		typedef empty argument_type;
		typedef empty result_type;
		
		template < class Putter >
		static void Put( argument_type toPut, const Putter& put )
		{
			put( 0, 0 );
		}
		
		template < class Getter >
		static result_type Get( const Getter& get )
		{
			get( 0, 0 );  // So it has a chance to throw
			
			return empty();
		}
		
		static const bool hasStaticSize = true;
		typedef empty Buffer;
	};
	
}

#endif

