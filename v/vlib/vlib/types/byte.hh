/*
	byte.hh
	-------
*/

#ifndef VLIB_TYPES_BYTE_HH
#define VLIB_TYPES_BYTE_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	struct type_info;
	
	extern const dispatch byte_dispatch;
	
	class Byte : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.type() == Value_byte;
			}
			
			static Value coerce( const Value& v );
			
		#define IBOX( i )  bignum::ibox( (unsigned long) i ).move()
			
			Byte( unsigned char c = 0 ) : Value( IBOX( c ),
			                                     Value_byte,
			                                     &byte_dispatch )
			{
			}
			
		#undef IBOX
			
			unsigned char get() const;
			
			operator unsigned char() const
			{
				return get();
			}
	};
	
	extern const type_info byte_vtype;
	
}

#endif
