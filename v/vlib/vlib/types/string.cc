/*
	string.cc
	---------
*/

#include "vlib/types/string.hh"

// more-libc
#include "more/string.h"

// vlib
#include "vlib/array-utils.hh"
#include "vlib/list-utils.hh"
#include "vlib/proc_info.hh"
#include "vlib/quote.hh"
#include "vlib/string-utils.hh"
#include "vlib/target.hh"
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/types/any.hh"
#include "vlib/types/byte.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/pointer.hh"
#include "vlib/types/proc.hh"
#include "vlib/types/type.hh"


#undef mempcpy


namespace vlib
{
	
	extern const proc_info proc_lines;
	
	using ::mempcpy;
	
	static inline
	char* mempcpy( char* p, const plus::string& s )
	{
		return (char*) mempcpy( p, s.data(), s.size() );
	}
	
	
	Value String::coerce( const Value& v )
	{
		return String( str( v ) );
	}
	
	static
	const char* string_str_data( const Value& v )
	{
		return v.string().data();
	}
	
	static
	size_t string_str_size( const Value& v )
	{
		return v.string().size();
	}
	
	static const stringify string_str =
	{
		&string_str_data,
		&string_str_size,
		NULL,
	};
	
	static
	size_t string_rep_size( const Value& v )
	{
		return quote_string( v.string() ).size();
	}
	
	static
	char* string_rep_copy( char* p, const Value& v )
	{
		return mempcpy( p, quote_string( v.string() ) );
	}
	
	static const stringify string_rep =
	{
		NULL,
		&string_rep_size,
		&string_rep_copy,
	};
	
	static const stringifiers string_stringifiers =
	{
		&string_str,
		&string_rep,
		&string_str,  // reuse str for bin
	};
	
	static
	Value unary_op_handler( op_type op, const Value& v )
	{
		switch ( op )
		{
			case Op_typeof:
				return Type( string_vtype );
			
			case Op_unary_minus:
				return reversed_bytes( v );
			
			case Op_each:
				return Pointer( v );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static
	Value binary_op_handler( op_type op, const Value& a, const Value& b )
	{
		switch ( op )
		{
			case Op_function:
			case Op_named_unary:
				return String( str( Value( a, b ) ) );
			
			case Op_format:
				return String( format( a.string(), b ) );
			
			default:
				break;
		}
		
		return Value();
	}
	
	Value assign_byte_to_index( Value& s, const Value& x, const Byte& byte )
	{
		Expr* expr = x.expr();
		
		const Value& subscript = expr->right;
		
		const unsigned i = subscript_integer( subscript );
		
		if ( i >= s.string().size() )
		{
			THROW( "subscript exceeds string/pack bounds" );
		}
		
		char* data = (char*) s.string().data();
		
		data[ i ] = byte;
		
		return byte;
	}
	
	static
	Value mutating_op_handler( op_type        op,
	                           const Target&  target,
	                           const Value&   x,
	                           const Value&   b )
	{
		switch ( op )
		{
			case Op_duplicate:
				if ( b.type() != Value_byte )
				{
					THROW( "can't assign non-byte to string element" );
				}
				// fall through
			
			case Op_approximate:
				return assign_byte_to_index( *target.addr, x, b.to< Byte >() );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static const operators ops =
	{
		&unary_op_handler,
		&binary_op_handler,
		NULL,
		&mutating_op_handler,
	};
	
	const dispatch string_dispatch =
	{
		&string_stringifiers,
		NULL,
		NULL,
		&ops,
	};
	
	static
	Value string_member( const Value& obj,
	                     const plus::string& member )
	{
		if ( member == "length" )
		{
			return Integer( obj.string().size() );
		}
		
		if ( member == "join" )
		{
			return bind_args( Proc( proc_join ), obj );
		}
		
		if ( member == "lines" )
		{
			return bind_args( Proc( proc_lines ), obj );
		}
		
		THROW( "nonexistent string member" );
		
		return Value();
	}
	
	const type_info string_vtype =
	{
		"string",
		&assign_to< String >,
		&String::coerce,
		&string_member,
	};
	
	static
	Value v_join( const Value& args )
	{
		const Value& glue = first( args );
		
		const Value pieces = rest( args );
		
		return String( join( glue.string(), pieces, count( pieces ) ) );
	}
	
	static
	Value v_lines( const Value& v )
	{
		return lines( v.string() );
	}
	
	static const Type  etc        = etc_vtype;
	static const Type  string     = string_vtype;
	static const Value string_etc = Value( string, etc );
	
	const proc_info proc_join  = { "join",  &v_join,  &string_etc };
	const proc_info proc_lines = { "lines", &v_lines, &string     };
	
}
