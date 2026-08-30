//
//  circuit.hpp
//  NAND
//
//  Created by Claude Opus 5 on 2026-08-30.
//  Copyright © 2026 Gabriel Beauchemin. All rights reserved.
//

#ifndef circuit_hpp
#define circuit_hpp

#include <cassert>
#include <utility>
#include <vector>

// A gate-level netlist, evaluated in sweeps rather than by pushing events
// along callbacks.
//
// The wire_t model assigns an input and evaluates the gate in the same call,
// so a signal that fans out to k gate inputs is applied one input at a time
// and the circuit passes through states that never exist in hardware.
// Combinational logic recovers from that, because the value it settles on is
// still right. Memory does not: a latch captures the transient and holds it,
// which is why a D latch built from wire_t NANDs cannot hold a bit.
//
// Here, assignment and evaluation are separate by construction. set() only
// writes a value; nothing is evaluated until settle() sweeps the netlist, so
// every fan-out is applied atomically and feedback works.
class circuit_t
{
public:
	using node = unsigned;

	static constexpr unsigned DEFAULT_BUDGET = 1000;

private:
	struct gate_t
	{
		std::vector<node> in;
		node              out;
	};

	// char rather than bool: std::vector<bool> hands out proxies, not refs.
	std::vector<char>   m_values;
	std::vector<gate_t> m_gates;

public:
	// A node nothing drives - an external input, written with set().
	node input(bool value = false)
	{
		m_values.push_back(value ? 1 : 0);
		return (node)m_values.size() - 1;
	}

	// A node that a gate will drive. Declared separately from the gate driving
	// it so that cross-coupled feedback can be wired up.
	node wire(bool value = true) { return input(value); }

	void nand_gate(node out, const std::vector<node>& ins)
	{
		assert(out < m_values.size());
		assert(!ins.empty());
		for (node i : ins) { assert(i < m_values.size()); (void)i; }

		m_gates.push_back({ins, out});
	}

	// NAND is the primitive. Everything below is composed from it.
	node nand_(const std::vector<node>& ins) { node o = wire(); nand_gate(o, ins); return o; }
	node not_(node a)                        { return nand_({a, a}); }
	node and_(node a, node b)                { return not_(nand_({a, b})); }
	node or_(node a, node b)                 { return nand_({not_(a), not_(b)}); }
	node nor_(node a, node b)                { return not_(or_(a, b)); }
	node xnor_(node a, node b)               { return not_(xor_(a, b)); }

	node xor_(node a, node b)
	{
		node n = nand_({a, b});
		return nand_({nand_({a, n}), nand_({n, b})});
	}

	void set(node n, bool value) { assert(n < m_values.size()); m_values[n] = value ? 1 : 0; }
	bool get(node n) const       { assert(n < m_values.size()); return m_values[n] != 0; }

	// Sweeps the netlist until nothing changes. One sweep is one gate delay,
	// so the return value is the settling time of whatever just changed.
	//
	// Gates are evaluated in place, so a gate sees values written by gates
	// earlier in the same sweep. That models unequal propagation delays. A
	// strictly simultaneous update (compute every gate from the old values,
	// then commit) agrees with this on well-behaved input, but spins forever
	// when an input changes in the same sweep as a clock edge: the
	// cross-coupled pair flips in lockstep and never converges. Evaluating in
	// place breaks that symmetry, so a setup violation yields a definite
	// answer - the pre-edge value - instead of a hang.
	//
	// Returns 0 if the circuit never settled - a ring oscillator, or a design
	// error. Callers should treat 0 as a failure, not ignore it.
	unsigned settle(unsigned budget = DEFAULT_BUDGET)
	{
		for (unsigned sweep = 1; sweep <= budget; sweep++)
		{
			bool changed = false;

			for (auto&& g : m_gates)
			{
				char value = 0;                     // NAND: low only when every input is high
				for (node i : g.in)
					if (!m_values[i]) { value = 1; break; }

				if (m_values[g.out] != value)
				{
					m_values[g.out] = value;
					changed = true;
				}
			}

			if (!changed)
				return sweep;
		}

		return 0;
	}

	std::size_t gates() const { return m_gates.size(); }
	std::size_t nodes() const { return m_values.size(); }
};


// A NAND SR latch, active low. Returns { q, qbar }.
inline std::pair<circuit_t::node, circuit_t::node>
sr_latch(circuit_t& c, circuit_t::node sbar, circuit_t::node rbar)
{
	circuit_t::node q    = c.wire(false);
	circuit_t::node qbar = c.wire(true);

	c.nand_gate(q,    {sbar, qbar});
	c.nand_gate(qbar, {rbar, q});

	return {q, qbar};
}

// A gated D latch: transparent while e is high, holds when it is low. Four
// NANDs. This is what dgate_t is standing in for today.
inline circuit_t::node dlatch(circuit_t& c, circuit_t::node d, circuit_t::node e)
{
	circuit_t::node n1 = c.nand_({d, e});
	circuit_t::node n2 = c.nand_({n1, e});

	circuit_t::node q    = c.wire(false);
	circuit_t::node qbar = c.wire(true);

	c.nand_gate(q,    {n1, qbar});
	c.nand_gate(qbar, {n2, q});

	return q;
}

// A positive-edge-triggered D flip-flop: two gated latches in master/slave on
// opposite clock phases, so data can never run through both in one phase.
inline circuit_t::node dff(circuit_t& c, circuit_t::node d, circuit_t::node clk)
{
	return dlatch(c, dlatch(c, d, c.not_(clk)), clk);
}

#endif /* circuit_hpp */
