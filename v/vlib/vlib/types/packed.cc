/*
	packed.cc
	---------
*/

#include "vlib/types/packed.hh"

// math
#include "math/bitwise.hh"

// gear
#include "gear/hexadecimal.hh"

// bignum
#include "bignum/integer.hh"

// vlib
#include "vlib/string-utils.hh"
#include "vlib/target.hh"
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/types/byte.hh"
#include "vlib/iterators/list_builder.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/pointer.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"


namespace vlib
{
	
	Value Packed::coerce( const Value& v )
	{
		switch ( v.type() )
		{
			case Value_packed:
				return v;
			
			default:
				THROW( "type can't be packed" );
			
			case Value_empty_list:
				return Packed();
			
			case Value_string:
				return Packed( v.string() );
			
			case Value_byte:
			case Value_mb32:
			case Value_pair:
			case Value_other:
				return Packed( pack( v ) );
		}
	}
	
	static
	size_t packed_str_size( const Value& v )
	{
		return v.string().size() * 2;
	}
	
	static
	char* packed_str_copy( char* p, const Value& v )
	{
		const plus::string& s = v.string();
		
		return gear::hexpcpy_lower( p, s.data(), s.size() );
	}
	
	static const stringify packed_str =
	{
		NULL,
		&packed_str_size,
		&packed_str_copy,
	};
	
	static
	size_t packed_rep_size( const Value& v )
	{
		return packed_str_size( v ) + 3;
	}
	
	static
	char* packed_rep_copy( char* p, const Value& v )
	{
		*p++ = 'x';
		*p++ = '"';
		
		p = packed_str_copy( p, v );
		
		*p++ = '"';
		
		return p;
	}
	
	static const stringify packed_rep =
	{
		NULL,
		&packed_rep_size,
		&packed_rep_copy,
	};
	
	static
	const char* packed_bin_data( const Value& v )
	{
		return v.string().data();
	}
	
	static
	size_t packed_bin_size( const Value& v )
	{
		return v.string().size();
	}
	
	static const stringify packed_bin =
	{
		&packed_bin_data,
		&packed_bin_size,
		NULL,
	};
	
	static const stringifiers packed_stringifiers =
	{
		&packed_str,
		&packed_rep,
		&packed_bin,
	};
	
	static
	Value negated_bytes( Value v )
	{
		using namespace math::bitwise;
		
		const plus::string& s = v.unshare().string();
		
		bitwise_not( (char*) s.data(), s.size() );
		
		return v;
	}
	
	static
	Value unary_op_handler( op_type op, const Value& v )
	{
		switch ( op )
		{
			case Op_typeof:
				return Type( packed_vtype );
			
			case Op_unary_minus:
				return reversed_bytes( v );
			
			case Op_unary_negate:
				return negated_bytes( v );
			
			case Op_each:
				return Pointer( v );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static
	Value binary_bitwise_op( op_type op, Value a, const char* b )
	{
		using namespace math::bitwise;
		
		const plus::string& s = a.unshare().string();
		
		char* data = (char*) s.data();
		
		const plus::string::size_type n = s.size();
		
		switch ( op )
		{
			default:  // won't happen
			
			case Op_intersection:  bitwise_and( data, b, n );  break;
			case Op_exclusion:     bitwise_xor( data, b, n );  break;
			case Op_union:         bitwise_or ( data, b, n );  break;
		}
		
		return a;
	}
	
	static
	Value division( const plus::string& s, bignum::integer x )
	{
		typedef bignum::ibox::sign_t     sign_t;
		typedef plus::string::size_type  size_t;
		
		const sign_t sign = x.sign();
		
		if ( sign == 0 )
		{
			THROW( "division by zero (of packed data)" );
		}
		
		x.absolve();  // in-place absolute value
		
		if ( ! x.demotes_to< size_t >() )
		{
			THROW( "divisor of packed data is excessively large" );
		}
		
		const size_t divisor = x.clipped_to< size_t >();
		const size_t n_bytes = s.size();
		
		const size_t quotient  = n_bytes / divisor;
		const size_t remainder = n_bytes % divisor;
		
		if ( remainder != 0 )
		{
			THROW( "division of packed data with non-zero remainder" );
		}
		
		/*
			Let S be the size of the input data in bytes.
			Let N be the divisor argument.
			Let Q be S/abs(N).
			
			Dividing by N > 0 yields N pieces of length Q.
			Dividing by N < 0 yields Q pieces of length -N.
		*/
		
		const size_t chunk_len = sign > 0 ? quotient
		                                  : divisor;
		
		size_t n_chunks = sign > 0 ? divisor
		                           : quotient;
		
		list_builder result;
		
		size_t i = 0;
		
		while ( n_chunks-- > 0 )
		{
			result.append( Packed( s.substr( i, chunk_len ) ) );
			
			i += chunk_len;
		}
		
		return result.get();
	}
	
	static
	Value binary_op_handler( op_type op, const Value& a, const Value& b )
	{
		switch ( op )
		{
			case Op_function:
			case Op_named_unary:
				return Packed( pack( Value( a, b ) ) );
			
			case Op_intersection:
			case Op_exclusion:
			case Op_union:
				if ( b.type() != V_pack )
				{
					THROW( "bitwise logic requires packed operands" );
				}
				
				if ( a.string().size() != b.string().size() )
				{
					THROW( "bitwise logic operands must be of equal length" );
				}
				
				return binary_bitwise_op( op, a, b.string().data() );
			
			case Op_divide:
				if ( const Integer* integer = b.is< Integer >() )
				{
					return division( a.string(), *integer );
				}
				
				THROW( "divisor of packed data must be an integer" );
			
			default:
				break;
		}
		
		return Value();
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
					THROW( "can't assign non-byte to packed element" );
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
	
	const dispatch packed_dispatch =
	{
		&packed_stringifiers,
		NULL,
		NULL,
		&ops,
	};
	
	static
	Value packed_member( const Value&         obj,
	                     const plus::string&  member )
	{
		if ( member == "size" )
		{
			return Integer( obj.string().size() );
		}
		
		if ( member == "string" )
		{
			return String( obj.string() );
		}
		
		THROW( "nonexistent packed data member" );
		
		return Value();
	}
	
	const type_info packed_vtype =
	{
		"packed",
		&assign_to< Packed >,
		&Packed::coerce,
		&packed_member,
	};
	
}
