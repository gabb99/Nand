//
//  xor.hpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-22.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef xor_hpp
#define xor_hpp

#include <bitset>

#include "nand.hpp"

template <unsigned N = 2>
class xor_t
{
	// The xor with two input can be easily expressed using nand_t gates
	// However, templating this to > 2 inputs, complexity ensues
	// For simplicity's sake, make this a basic gate
	
	// Also xor with more than 2 inputs is ambigous
	// https://electronics.stackexchange.com/questions/93713/how-is-an-xor-with-more-than-2-inputs-supposed-to-work
	// We'll be using the "1 and only 1" over the "odd-parity function"

	std::array<bool, N> m_inputs;
	wire_t<1> m_output;
	
public:
	xor_t() { m_inputs.fill(false); }
	
	void attach(const std::function<void(bool)>& cb)
	{
		m_output.attach(cb);
	}
	
	void in(const std::bitset<N>& b)
	{
		for (unsigned i = 0; i < N; i++)
			m_inputs[i] = b.test(i);
		
		m_output.in(out());
	}
	
	void in(const std::initializer_list<bool>& in)
	{
		assert(in.size() == N);
		
		for (unsigned i = 0; i < in.size(); i++)
			m_inputs[i] = in.begin()[i];
		
		m_output.in(out());
	}
	
	void in(bool value, unsigned in)
	{
		assert(in < m_inputs.size());
		m_inputs[in] = value;
		m_output.in(out());
	}
	
	bool out() const
	{
		unsigned cnt = 0;
		for (auto&& i  : m_inputs)
			if (i)
				cnt++;

		return cnt == 1;
	}
};

#endif /* xor_hpp */
