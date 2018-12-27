//
//  xnor.hpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-22.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef xnor_hpp
#define xnor_hpp

#include "not.hpp"
#include "xor.hpp"

template <unsigned N = 2>
class xnor_t
{
	xor_t<N>	m_nxor;
	not_t m_not;
	
public:
	xnor_t()
	{
		m_nxor.attach([&](bool value) { m_not.in(value); } );
	}
	
	void attach(const std::function<void(bool)>& cb)
	{
		m_not.attach(cb);
	}
	
	void in(const std::bitset<N>& b)
	{
		m_nxor.in(b);
	}
	
	void in(const std::initializer_list<bool>& in)
	{
		m_nxor.in(in);
	}
	
	void in(bool value, unsigned in)
	{
		m_nxor.in(value, in);
	}
	
	bool out() const
	{
		return m_not.out();
	}
};

#endif /* xnor_hpp */
