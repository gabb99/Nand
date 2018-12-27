//
//  enabler.hpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-23.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef enabler_hpp
#define enabler_hpp

#include "and.hpp"

template <unsigned N = 1>
class enabler_t
{
	std::array<and_t<2>, N> m_ands;
	
public:
	enabler_t() {}
	
	void attach(const std::function<void(bool)>& cb, unsigned n = 0)
	{
		m_ands[n].attach(cb);
	}
	
	void enable(bool value)
	{
		for (auto&& n : m_ands)
			n.in(value, 1);
	}
	
	void in(const std::bitset<N>& b)
	{
		for (unsigned i = 0; i < N; i++)
			m_ands[i].in(b[i], 0);
	}
	
	void in(const std::initializer_list<bool>& in)
	{
		assert(in.size() == N);
		
		for (unsigned i = 0; i < N; i++)
			m_ands[i].in(in.begin()[i], 0);
	}
	
	void in(bool value, unsigned n)
	{
		assert(n < m_ands.size());
		m_ands[n].in(value, 0);
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
		return m_ands[n].out();
	}
};

#endif /* enabler_hpp */
