//
//  adder.hpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-23.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef adder_hpp
#define adder_hpp

#include "and.hpp"
#include "xor.hpp"

template <unsigned N = 2>
class adder_t
{
	class adder_1_t
	{
		and_t<2> m_and;
		xor_t<2> m_xor;
		
	public:
		adder_1_t() {}

		void a(bool value)
		{
			m_and.in(value, 0);
			m_xor.in(value, 0);
		}

		void b(bool value)
		{
			m_and.in(value, 1);
			m_xor.in(value, 1);
		}

		bool sum() const
		{
			return m_xor.out();
		}

		bool carry() const
		{
			return m_and.out();
		}
	};

	std::array<adder_1_t, N> m_adders;
	
public:
	adder_t() {}
	
	void attach(const std::function<void(bool)>& cb, unsigned n = 0)
	{
        m_adders[n].attach(cb);
	}
	
	void enable(bool value)
	{
		for (auto&& n : m_adders)
			n.in(value, 1);
	}
	
	void in(const std::bitset<N>& b)
	{
		for (unsigned i = 0; i < N; i++)
            m_adders[i].in(b[i], 0);
	}
	
	void in(const std::initializer_list<bool>& in)
	{
		assert(in.size() == N);
		
		for (unsigned i = 0; i < N; i++)
            m_adders[i].in(in.begin()[i], 0);
	}
	
	void in(bool value, unsigned n)
	{
		assert(n < m_adders.size());
        m_adders[n].in(value, 0);
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
		return m_adders[n].out();
	}
};

#endif /* adder_hpp */
