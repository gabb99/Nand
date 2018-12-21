//
//  nand.hpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-20.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef nand_h
#define nand_h

#include "wire.hpp"

template <unsigned N = 2>
class nand_t
{
	std::array<bool, N> m_inputs;
	wire_t<1> m_output;

	void signal()
	{
		m_output.in(out());
	}

public:
	nand_t() {}
	
	unsigned inputs() const { return N; }

	void attach(const std::function<void(bool)>& cb)
	{
		m_output.attach(cb);
	}

	void in(const std::initializer_list<bool>& in)
	{
		assert(in.size() == N);

		for (unsigned i = 0; i < in.size(); i++)
			m_inputs[i] = in.begin()[i];

		signal();
	}

	void in(bool value, unsigned in)
	{
		m_inputs[in] = value;
		signal();
	}
	
	bool out() const
	{
		bool and_ = true;
		for (auto&& i  : m_inputs)
			if (i == false)
			{
				and_ = false;
				break;
			}
		
		return !and_;
	}
};

#endif /* nand_h */
