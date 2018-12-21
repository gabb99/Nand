//
//  wire.hpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-20.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef wire_h
#define wire_h

#include <functional>
#include <array>

template <unsigned N = 1>
class wire_t
{
	std::array<std::function<void(bool)>, N> m_wires;

	void out(bool value)
	{
		for (auto&& w : m_wires)
			if (w)
				w(value);
	}

public:
	wire_t() {}

	unsigned size() const { return N; }

	void attach(const std::function<void(bool)>& cb, unsigned w = 0)
	{
		m_wires[w] = cb;
	}

	void attach(const wire_t<N>& w)
	{
		m_wires = w.m_wires;
	}

	void in(bool value)
	{
		out(value);
	}
};

#endif /* wire_h */
