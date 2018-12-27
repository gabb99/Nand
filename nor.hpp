//
//  nor.hpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-22.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef nor_hpp
#define nor_hpp

#include "not.hpp"
#include "or.hpp"

template <unsigned N = 2>
class nor_t
{
	or_t<N>	m_or;
	not_t m_not;
	
public:
	nor_t()
	{
		m_or.attach([&](bool value) { m_not.in(value); } );
	}
	
	void attach(const std::function<void(bool)>& cb)
	{
		m_not.attach(cb);
	}
	
	void in(const std::bitset<N>& b)
	{
		m_or.in(b);
	}
	
	void in(const std::initializer_list<bool>& in)
	{
		m_or.in(in);
	}
	
	void in(bool value, unsigned n)
	{
		m_or.in(value, n);
	}
	
	bool out() const
	{
		return m_not.out();
	}
};

#endif /* nor_hpp */
