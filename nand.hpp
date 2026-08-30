//
//  nand.hpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-20.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef nand_h
#define nand_h

#include <cassert>
#include <bitset>

#include "wire.hpp"

template <unsigned N = 2>
class nand_t
{
	std::array<bool, N> m_inputs;
	wire_t<1> m_output;

public:
	nand_t() { m_inputs.fill(false); }
	
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

	void in(bool value, unsigned n)
	{
		assert(n < m_inputs.size());
		m_inputs[n] = value;
		m_output.in(out());
	}
	
	bool out() const
	{
		bool and_ = true;
		for (auto&& i  : m_inputs)
			if (i == false)
			{
				and_ = false;
				break;
			}
		
		return !and_;
	}
};

#endif /* nand_h */
