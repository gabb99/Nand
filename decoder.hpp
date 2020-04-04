//
//  decoder.hpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-23.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef decoder_hpp
#define decoder_hpp

#include "and.hpp"
#include "not.hpp"

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
		assert(n < m_ands.size());
		m_ands[n<<1].in(value, 0);
		
		// Could also have fed directly the m_nots[n].out()
		m_nots[n].in(value);
	}

public:
	decoder_t()
	{
		for (unsigned i = 0; i < N; i++)
		{
			m_nots[i].attach([&, i](bool value) { m_ands[i<<1].in(value, 1); } );
		}
	}
	
	void attach(const std::function<void(bool)>& cb, unsigned n = 0)
	{
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
		return m_ands[n].out();
	}
};

#endif /* decoder_hpp */
