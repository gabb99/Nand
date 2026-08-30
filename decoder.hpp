//
//  decoder.hpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-23.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef decoder_hpp
#define decoder_hpp

#include <cassert>

#include "and.hpp"
#include "not.hpp"

// An N-to-2^N decoder: exactly one of the SIZE output lines is high, the one
// whose index equals the value on the N input bits.
//
// Output line i is the minterm for i, so its AND gate wants, on port n, the
// input bit n when bit n of i is set and the complement of that bit when it is
// not. One inverter per input bit feeds every gate that wants the complement.
//
// Bit n is the input at index n, least significant first, matching the
// std::bitset overload of in().
template <unsigned N = 2>
class decoder_t
{
public:
	constexpr static unsigned SIZE = 1<<N;

private:
	std::array<and_t<N>, SIZE> m_ands;
	std::array<not_t, N> m_nots;

	void in_impl(bool value, unsigned n)
	{
		assert(n < N);

		// The gates that want bit n straight.
		for (unsigned i = 0; i < SIZE; i++)
			if ((i >> n) & 1)
				m_ands[i].in(value, n);

		// m_nots[n] fans the complement out to the rest.
		m_nots[n].in(value);
	}

public:
	decoder_t()
	{
		for (unsigned n = 0; n < N; n++)
			for (unsigned i = 0; i < SIZE; i++)
				if (((i >> n) & 1) == 0)
					m_nots[n].attach([this, i, n](bool value) { m_ands[i].in(value, n); });

		// Settle the gates for the power-on input of all zeros, so that out()
		// is meaningful before the first in().
		for (unsigned n = 0; n < N; n++)
			in_impl(false, n);
	}
	
	void attach(const std::function<void(bool)>& cb, unsigned n = 0)
	{
		assert(n < SIZE);
		m_ands[n].attach(cb);
	}
	
	void in(const std::bitset<N>& b)
	{
		for (unsigned i = 0; i < N; i++)
			in_impl(b[i], i);
	}
	
	void in(const std::initializer_list<bool>& in)
	{
		assert(in.size() == N);
		
		for (unsigned i = 0; i < N; i++)
			in_impl(in.begin()[i], i);
	}
	
	void in(bool value, unsigned n)
	{
		in_impl(value, n);
	}
	
	std::bitset<SIZE> out() const
	{
		std::bitset<SIZE> n;
		
		for (unsigned i = 0; i < n.size(); i++)
			n.set(i, out(i));
		
		return n;
	}
	
	bool out(unsigned n) const
	{
		assert(n < SIZE);
		return m_ands[n].out();
	}
};

#endif /* decoder_hpp */
