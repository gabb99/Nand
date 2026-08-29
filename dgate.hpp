//
//  dgate.hpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-20.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef dgate_h
#define dgate_h

#include <bitset>
#include <cassert>

#include "wire.hpp"

template <unsigned N = 1>
class dgate_t
{
	// Usually expressed using nand gates, but because of the feedback
	// cannot be made to work in a discrete mode.
	// For simplicity's sake, make this a basic gate

	std::array<bool, N> m_inputs;
	std::array<bool, N> m_gated_inputs;
	bool m_set = false;
	wire_t<N> m_output;
	
public:
	dgate_t() { m_inputs.fill(false); m_gated_inputs.fill(false); }
	
	void attach(const std::function<void(bool)>& cb, unsigned w = 0)
	{
		m_output.attach(cb, w);
	}

	void set(bool value)
	{
		m_set = value;
		if (value)
		{
			m_gated_inputs = m_inputs;

			for (unsigned i = 0; i < N; i++)
				m_output.in(out(i), i);
		}
	}

	void in(const std::bitset<N>& b)
	{
		for (unsigned i = 0; i < N; i++)
		{
			m_inputs[i] = b.test(i);
			if (m_set) m_gated_inputs[i] = m_inputs[i];
			m_output.in(out(i), i);
		}
	}
	
	void in(const std::initializer_list<bool>& in)
	{
		assert(in.size() == N);
		
		for (unsigned i = 0; i < in.size(); i++)
		{
			m_inputs[i] = in.begin()[i];
			if (m_set) m_gated_inputs[i] = m_inputs[i];
			m_output.in(out(), i);
		}
	}
	
	void in(bool value, unsigned n)
	{
		assert(n < m_inputs.size());
		m_inputs[n] = value;
		if (m_set) m_gated_inputs[n] = m_inputs[n];
		m_output.in(out(n), n);
	}

	std::bitset<N> out() const
	{
		std::bitset<N> n;

		for (unsigned i = 0; i < N; i++)
			n.set(i, out(i));

		return n;
	}

	bool out(unsigned n) const
	{
		if (m_set == false)
			return m_gated_inputs[n];

		return m_inputs[n];
	}
};

#endif /* dgate_h */
