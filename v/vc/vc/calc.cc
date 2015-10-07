/*
	calc.cc
	-------
*/

#include "vc/calc.hh"

// plus
#include "plus/decimal.hh"
#include "plus/hexadecimal.hh"
#include "plus/integer_hex.hh"
#include "plus/string/concat.hh"

// vc
#include "vc/error.hh"
#include "vc/function_id.hh"


namespace vc
{
	
	using math::integer::cmp_t;
	
	
	static plus::string hex( const plus::string& s )
	{
		return plus::hex_lower( s.data(), s.size() );
	}
	
	static
	const plus::integer& nonzero( const plus::integer& x )
	{
		if ( x.is_zero() )
		{
			DOMAIN_ERROR( "division by zero" );
		}
		
		return x;
	}
	
	static
	bool equal( const Value& a, const Value& b )
	{
		if ( a.type != b.type )
		{
			if ( a.type == Value_empty_list  ||  b.type == Value_empty_list )
			{
				return false;
			}
			
			TYPE_ERROR( "mismatched types in equality relation" );
		}
		
		switch ( a.type )
		{
			case Value_empty_list:
				return true;
			
			case Value_boolean:
			case Value_number:
				return a.number == b.number;
			
			case Value_string:
				return a.string == b.string;
			
			case Value_pair:
				SYNTAX_ERROR( "equality for lists unimplemented" );
				break;
			
			default:
				INTERNAL_ERROR( "unsupported type in equal()" );
				break;
		}
		
		return false;
	}
	
	static
	cmp_t compare( const Value& a, const Value& b )
	{
		if ( a.type != b.type )
		{
			TYPE_ERROR( "mismatched types in compare()" );
		}
		
		switch ( a.type )
		{
			case Value_number:
				return compare( a.number, b.number );
			
			case Value_string:
				return compare( a.string, b.string );
			
			default:
				TYPE_ERROR( "unsupported type in compare()" );
				break;
		}
		
		return 0;
	}
	
	static
	plus::string make_string( const Value& value )
	{
		switch ( value.type )
		{
			case Value_empty_list:  // ""
			case Value_string:
			case Value_function:
				return value.string;
			
			case Value_boolean:
				return value.number.is_zero() ? "false" : "true";
			
			case Value_number:
				return encode_decimal( value.number );
			
			default:
				break;
		}
		
		if ( Expr* expr = value.expr.get() )
		{
			return make_string( expr->left ) + make_string( expr->right );
		}
		
		INTERNAL_ERROR( "unsupported type in make_string()" );
		
		return plus::string::null;
	}
	
	static
	Value v_bool( const Value& arg )
	{
		switch ( arg.type )
		{
			default:
				INTERNAL_ERROR( "invalid type in v_bool()" );
			
			case Value_empty_list:
				return false;
			
			case Value_boolean:
				return arg;
			
			case Value_number:
				return ! arg.number.is_zero();
			
			case Value_string:
				return ! arg.string.empty();
			
			case Value_function:
			case Value_pair:
				return true;
		}
	}
	
	static
	Value v_hex( const Value& arg )
	{
		switch ( arg.type )
		{
			default:  TYPE_ERROR( "invalid argument to hex()" );
			
			case Value_number:  return hex( arg.number );
			case Value_string:  return hex( arg.string );
		}
	}
	
	static
	Value calc_function( unsigned f, Value arg )
	{
		if ( f == Function_str )
		{
			return make_string( arg );
		}
		
		if ( f == Function_bool )
		{
			return v_bool( arg );
		}
		
		if ( f == Function_hex )
		{
			return v_hex( arg );
		}
		
		switch ( arg.type )
		{
			case Value_empty_list:
				SYNTAX_ERROR( "function unimplemented for empty list" );
				break;
			
			case Value_boolean:
				SYNTAX_ERROR( "function unimplemented for boolean values" );
				break;
			
			case Value_number:
				switch ( f )
				{
					case Function_abs:   arg.number.absolve();  break;
					case Function_half:  arg.number.halve();    break;
					
					default:
						INTERNAL_ERROR( "unimplemented function" );
				}
				
				break;
			
			case Value_string:
				SYNTAX_ERROR( "function unimplemented for strings" );
				break;
			
			case Value_pair:
				SYNTAX_ERROR( "function unimplemented for lists" );
				break;
			
			default:
				INTERNAL_ERROR( "invalid type in calc_function()" );
		}
		
		return arg;
	}
	
	static
	Value calc_unary( op_type op, const Value& v )
	{
		if ( op == Op_const  ||  op == Op_var )
		{
			SYNTAX_ERROR( "const/var operand not a symbol" );
		}
		
		switch ( v.type )
		{
			case Value_empty_list:
			case Value_boolean:
			case Value_number:
				switch ( op )
				{
					case Op_unary_plus:   return  v.number;
					case Op_unary_minus:  return -v.number;
					
					default:  break;
				}
				
				break;
			
			case Value_string:
				SYNTAX_ERROR( "unary operator not defined for string values" );
				break;
			
			case Value_pair:
				SYNTAX_ERROR( "unary operator not defined for lists" );
				break;
			
			default:
				break;
		}
		
		INTERNAL_ERROR( "unsupported operator in calc_unary()" );
		
		return Value();
	}
	
	static
	Value calc( const plus::integer&  left,
	            op_type               op,
	            const plus::integer&  right )
	{
		switch ( op )
		{
			case Op_add:       return left + right;
			case Op_subtract:  return left - right;
			case Op_multiply:  return left * right;
			case Op_divide:    return left / nonzero( right );
			case Op_remain:    return left % nonzero( right );
			case Op_modulo:    return modulo( left, nonzero( right ) );
			
			case Op_empower:   return raise_to_power( left, right );
			
			default:
				break;
		}
		
		INTERNAL_ERROR( "unsupported operator in calc()" );
		
		return Value();
	}
	
	static
	Value make_pair( const Value& left, const Value& right )
	{
		if ( left.type != Value_pair )
		{
			return Value( left, right );
		}
		
		Expr& expr = *left.expr.get();
		
		return Value( expr.left, make_pair( expr.right, right ) );
	}
	
	Value calc( const Value&  left,
	            op_type       op,
	            const Value&  right )
	{
		if ( left.type == Value_dummy_operand )
		{
			return calc_unary( op, right );
		}
		
		switch ( op )
		{
			case Op_function:
			case Op_named_unary:
				if ( left.type == Value_function )
				{
					return left.function( right );
				}
				
				if ( left.type != Value_builtin_function )
				{
					SYNTAX_ERROR( "attempted call of non-function" );
				}
				
				return calc_function( left.number.clipped(), right );
			
			case Op_equal:    return equal( left, right );
			case Op_unequal:  return ! equal( left, right );
			
			case Op_lt:   return compare( left, right ) <  0;
			case Op_lte:  return compare( left, right ) <= 0;
			case Op_gt:   return compare( left, right ) >  0;
			case Op_gte:  return compare( left, right ) >= 0;
			
			default:  break;
		}
		
		if ( op == Op_list )
		{
			if ( left.type == Value_empty_list )
			{
				return right;
			}
			
			if ( right.type == Value_empty_list )
			{
				return left;
			}
			
			return make_pair( left, right );
		}
		
		if ( left.type == right.type )
		{
			switch ( left.type )
			{
				case Value_empty_list:
					SYNTAX_ERROR( "operator not defined for empty list" );
				
				case Value_boolean:
					SYNTAX_ERROR( "operator not defined for boolean values" );
				
				case Value_number:
					return calc( left.number, op, right.number );
				
				case Value_string:
					SYNTAX_ERROR( "operator not defined for string values" );
				
				case Value_pair:
					SYNTAX_ERROR( "operator not defined for lists" );
				
				default:
					break;
			}
		}
		
		SYNTAX_ERROR( "operator not defined on mixed types" );
		
		return Value();
	}
	
}
