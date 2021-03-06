/*
	concept.cc
	----------
*/

#include "channel/concept.hh"

// vlib
#include "vlib/throw.hh"
#include "vlib/types.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/stdint.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"

// vx
#include "channel/channel.hh"


namespace vlib
{
	
	static
	const char* channelconcept_str_data( const Value& v )
	{
		return "channel";
	}
	
	static const stringify channelconcept_str =
	{
		&channelconcept_str_data,
		NULL,
		NULL,
	};
	
	const stringifiers channelconcept_stringifiers =
	{
		&channelconcept_str,
		// rep: ditto
		// bin: not defined
	};
	
	static
	Value unary_op_handler( op_type op, const Value& v )
	{
		switch ( op )
		{
			case Op_typeof:
				return Type( type_vtype );
			
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
				if ( is_empty_list( b ) )
				{
					return Channel();
				}
				THROW( "invalid channel argument" );
			
			case Op_subscript:
				THROW( "channel subtypes are TBD" );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static const operators ops =
	{
		&unary_op_handler,
		&binary_op_handler,
	};
	
	const dispatch channelconcept_dispatch =
	{
		&channelconcept_stringifiers,
		NULL,
		NULL,
		&ops,
	};
	
}
