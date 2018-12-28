//
//  register.hpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-23.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef register_hpp
#define register_hpp

#include "dgate.hpp"
#include "enabler.hpp"

template <unsigned N = 1>
class registr_t
{
	dgate_t<N>   m_byte;
	enabler_t<N> m_enabler;

public:
	registr_t()
	{
		for (unsigned i = 0; i < N; i++)
		{
			m_byte.attach([&, i](bool value) { m_enabler.in(value, i); }, i);
		}
	}
	
	void attach(const std::function<void(bool)>& cb, unsigned n = 0)
	{
		m_enabler.attach(cb, n);
	}

	void set(bool value)
	{
		m_byte.set(value);
	}

	void enable(bool value)
	{
		m_enabler.enable(value);
	}
	

	void in(const std::bitset<N>& b)
	{
		m_byte.in(b);
	}
	
	void in(const std::initializer_list<bool>& in)
	{
		m_byte.in(in);
	}
	
	void in(bool value, unsigned n)
	{
		m_byte.in(value, n);
	}
	

	std::bitset<N> out() const
	{
		return m_enabler.out();
	}
	
	bool out(unsigned n) const
	{
		return m_enabler.out(n);
	}
};

#endif /* register_hpp */
