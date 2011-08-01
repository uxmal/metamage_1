/*
	fetch.cc
	--------
*/

#include "v68k/fetch.hh"

// v68k
#include "v68k/state.hh"


namespace v68k
{
	
	uint16_t fetch_instruction_word( processor_state& s )
	{
		const uint16_t word = s.mem.get_instruction_word( s.regs.pc );
		
		s.regs.pc += 2;
		
		return word;
	}
	
	
	uint32_t fetch_pc( processor_state& s )
	{
		return s.regs.pc;
	}
	
	uint32_t fetch_unsigned_word( processor_state& s )
	{
		return fetch_instruction_word( s );
	}
	
	uint32_t fetch_word_displacement( processor_state& s )
	{
		const int16_t word = fetch_instruction_word( s );
		
		return int32_t( word );
	}
	
	uint32_t fetch_longword_displacement( processor_state& s )
	{
		const uint32_t high = fetch_instruction_word( s );
		
		return high << 16 | fetch_instruction_word( s );
	}
	
	
	uint32_t fetch_data_at_1E00( processor_state& s )
	{
		return s.opcode >> 9 & 0x000F;
	}
	
	uint32_t fetch_data_at_000F( processor_state& s )
	{
		return s.opcode & 0x000F;
	}
	
	
	uint32_t fetch_data_at_0E00( processor_state& s )
	{
		return s.opcode >> 9 & 0x0007;
	}
	
	uint32_t fetch_data_at_0007( processor_state& s )
	{
		return s.opcode & 0x0007;
	}
	
	
	uint32_t fetch_MOVEP_opmode( processor_state& s )
	{
		return s.opcode >> 6 & 0x0007;
	}
	
	
	uint32_t fetch_signed_data_at_00FF( processor_state& s )
	{
		return int32_t( int8_t( s.opcode & 0x00ff ) );
	}
	
	
	uint32_t fetch_EXG_opmode( processor_state& s )
	{
		return s.opcode >> 3 & 0x001f;
	}
	
}

