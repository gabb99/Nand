//
//  or.hpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-20.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef or_hpp
#define or_hpp

#include "not.hpp"

template <unsigned N = 2>
class or_t
{
	std::array<not_t, N> m_not;
	nand_t<N>	m_nand;
	
public:
	or_t()
	{
		unsigned i = 0;
		for (auto&& e : m_not)
		{
			e.attach([&, i](bool value) { m_nand.in(value, i); } );
			++i;
		}
	}
	
	unsigned inputs() const { return N; }
	
	void attach(const std::function<void(bool)>& cb)
	{
		m_nand.attach(cb);
	}
	
	void in(const std::bitset<N>& b)
	{
		for (unsigned i = 0; i < N; i++)
			m_not[i].in(b.test(i));
	}
	
	void in(const std::initializer_list<bool>& in)
	{
		assert(in.size() == N);
		
		for (unsigned i = 0; i < in.size(); i++)
			m_not[i].in(in.begin()[i]);
	}
	
	void in(bool value, unsigned in)
	{
		assert(in < m_not.size());
		m_not[in].in(value);
	}
	
	bool out() const
	{
		return m_nand.out();
	}
};

#endif /* or_hpp */
