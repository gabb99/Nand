//
//  and.hpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-20.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef and_hpp
#define and_hpp

#include "not.hpp"

template <unsigned N = 2>
class and_t
{
	nand_t<N>	m_nand;
	not_t m_not;
	
public:
	and_t()
	{
		m_nand.attach([&](bool value) { m_not.in(value); } );
	}
	
	void attach(const std::function<void(bool)>& cb)
	{
		m_not.attach(cb);
	}

	void in(const std::bitset<N>& b)
	{
		m_nand.in(b);
	}

	void in(const std::initializer_list<bool>& in)
	{
		m_nand.in(in);
	}
	
	void in(bool value, unsigned n)
	{
		m_nand.in(value, n);
	}
	
	bool out() const
	{
		return m_not.out();
	}
};

#endif /* and_hpp */
