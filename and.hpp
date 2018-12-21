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
	}
	
	unsigned inputs() const { return N; }
	
	void attach(const std::function<void(bool)>& cb)
	{
		m_not.attach(cb);
	}
	
	void in(bool value, unsigned in)
	{
		m_nand.in(value, in);
	}
	
	bool out() const
	{
		return m_not.out();
	}
};

#endif /* and_hpp */
