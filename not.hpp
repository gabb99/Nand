//
//  not.hpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-20.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef not_h
#define not_h

#include "nand.hpp"

class not_t
{
	nand_t<2>	m_nand;
	
public:
	not_t() {}
	
	void attach(const std::function<void(bool)>& cb)
	{
		m_nand.attach(cb);
	}
	
	void in(bool value)
	{
		// Bind input together
		m_nand.in({value, value});
	}
	
	bool out() const
	{
		return m_nand.out();
	}
};

#endif /* not_h */
