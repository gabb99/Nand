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
#include <vector>
#include <array>

template <unsigned N = 1>
class wire_t
{
	// N input, X fanout
	std::array<std::vector<std::function<void(bool)>>, N> m_wires;

	void out(bool value, unsigned w)
	{
		for (auto&& w : m_wires[w])
			w(value);
	}

public:
	wire_t() {}

	unsigned size() const { return N; }

	void attach(const std::function<void(bool)>& cb, unsigned w = 0)
	{
		m_wires[w].push_back(cb);
	}

	void attach(const wire_t<N>& w)
	{
		m_wires = w.m_wires;
	}

	void in(bool value, unsigned w = 0)
	{
		out(value, w);
	}
};

#endif /* wire_h */
