//
//  dgate.hpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-20.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef dgate_h
#define dgate_h

#include <array>
#include <bitset>
#include <cassert>

#include "circuit.hpp"
#include "wire.hpp"

// N gated D latches sharing one enable line, built from NAND gates.
//
// Transparent while set is high - Q follows D - and holding the last value
// seen when it goes low. Four NANDs per bit, cross-coupled, the standard
// arrangement; see dlatch() in circuit.hpp.
//
// This used to be a plain std::array<bool> with a note saying feedback could
// not work in discrete mode. Feedback is not the obstacle. The obstacle was
// that wire_t assigns a gate input and evaluates the gate in the same call,
// so a signal fanning out to k inputs is applied one input at a time and the
// circuit passes through states that never exist in hardware. Combinational
// logic settles on the right answer anyway; a latch captures the transient
// and keeps it, which is why a D latch could not hold a bit.
//
// The latches therefore live in a circuit_t, where assignment and evaluation
// are separate: every write lands before anything is evaluated, so fan-out is
// atomic, and nothing reaches the output wires until the netlist has settled,
// so no glitch escapes the boundary.
template <unsigned N = 1>
class dgate_t
{
	circuit_t                      m_circuit;
	circuit_t::node                m_set;
	std::array<circuit_t::node, N> m_d;
	std::array<circuit_t::node, N> m_q;
	wire_t<N>                      m_output;

	void settle()
	{
		const unsigned sweeps = m_circuit.settle();
		assert(sweeps != 0 && "latches failed to settle");
		(void)sweeps;
	}

	void settle_and_drive()
	{
		settle();

		for (unsigned i = 0; i < N; i++)
			m_output.in(m_circuit.get(m_q[i]), i);
	}

public:
	dgate_t()
	{
		m_set = m_circuit.input(false);

		for (unsigned i = 0; i < N; i++)
		{
			m_d[i] = m_circuit.input(false);
			m_q[i] = dlatch(m_circuit, m_d[i], m_set);
		}

		settle();
	}

	void attach(const std::function<void(bool)>& cb, unsigned w = 0)
	{
		m_output.attach(cb, w);
	}

	void set(bool value)
	{
		m_circuit.set(m_set, value);
		settle_and_drive();
	}

	void in(const std::bitset<N>& b)
	{
		for (unsigned i = 0; i < N; i++)
			m_circuit.set(m_d[i], b.test(i));

		settle_and_drive();
	}

	void in(const std::initializer_list<bool>& in)
	{
		assert(in.size() == N);

		for (unsigned i = 0; i < N; i++)
			m_circuit.set(m_d[i], in.begin()[i]);

		settle_and_drive();
	}

	void in(bool value, unsigned n)
	{
		assert(n < N);

		m_circuit.set(m_d[n], value);
		settle_and_drive();
	}

	std::bitset<N> out() const
	{
		std::bitset<N> b;

		for (unsigned i = 0; i < N; i++)
			b.set(i, out(i));

		return b;
	}

	bool out(unsigned n) const
	{
		assert(n < N);
		return m_circuit.get(m_q[n]);
	}

	// How many NAND gates this costs, for the tests that care.
	std::size_t gates() const { return m_circuit.gates(); }
};

#endif /* dgate_h */
