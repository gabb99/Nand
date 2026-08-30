//
//  clock.hpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-20.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//
//  Implemented by Claude Opus 5 on 2026-08-30.
//

#ifndef clock_h
#define clock_h

#include "circuit.hpp"

// The machine's clock: one square wave plus the three signals derived from it.
//
//     clk          the raw wave
//     clk_delayed  the same wave, lagging
//     clk_set      clk AND clk_delayed   - a narrow pulse, late in the cycle
//     clk_enable   clk OR  clk_delayed   - a wide pulse that brackets it
//
//     phase           0     1     2     3
//     clk             _____|-----------|_____
//     clk_delayed     ___________|-----------|
//     clk_enable      _____|-----------------|
//     clk_set         ___________|-----|______
//
// The stagger is the point. clk_enable comes up first, so a source register is
// already driving the bus before clk_set pulses and a destination register
// captures; clk_set drops before clk_enable does, so the capture ends while
// the bus is still being driven. Without it, a register can latch a bus that
// nothing is driving yet.
//
// clk_delayed is a driven input rather than clk run through a delay chain.
// settle() runs the netlist to a fixed point, which erases combinational
// propagation delay - a chain of inverters on clk would settle to clk, and
// the stagger would vanish. So the lag lives where it can survive: in the
// order the generator drives the two inputs. One step() is one phase.
//
// Named clk_t, not clock_t: <ctime> already owns that name. The same dodge
// registr_t makes around the register keyword.
//
// clk_t adds its gates to a circuit shared with everything it drives, so it
// takes the circuit by reference rather than owning one.
class clk_t
{
	circuit_t&      m_circuit;
	circuit_t::node m_clk;
	circuit_t::node m_clk_delayed;
	circuit_t::node m_clk_set;
	circuit_t::node m_clk_enable;
	unsigned        m_phase = 0;

	unsigned drive()
	{
		m_circuit.set(m_clk,         m_phase == 1 || m_phase == 2);
		m_circuit.set(m_clk_delayed, m_phase == 2 || m_phase == 3);

		return m_circuit.settle();
	}

public:
	static constexpr unsigned PHASES = 4;

	// Build the rest of the machine, then call reset() to settle it.
	explicit clk_t(circuit_t& circuit)
		: m_circuit(circuit)
		, m_clk(circuit.input(false))
		, m_clk_delayed(circuit.input(false))
		, m_clk_set(circuit.and_(m_clk, m_clk_delayed))
		, m_clk_enable(circuit.or_(m_clk, m_clk_delayed))
	{
	}

	circuit_t::node clk() const         { return m_clk; }
	circuit_t::node clk_delayed() const { return m_clk_delayed; }
	circuit_t::node clk_set() const     { return m_clk_set; }
	circuit_t::node clk_enable() const  { return m_clk_enable; }

	unsigned phase() const { return m_phase; }

	// Back to phase 0 and settle. Returns sweeps taken, 0 if it never settled.
	unsigned reset()
	{
		m_phase = 0;
		return drive();
	}

	// Advance one phase and settle.
	unsigned step()
	{
		m_phase = (m_phase + 1) % PHASES;
		return drive();
	}

	// One full cycle. Returns the worst settling time of the four phases, or
	// 0 if any of them failed to settle.
	unsigned tick()
	{
		unsigned worst = 0;

		for (unsigned i = 0; i < PHASES; i++)
		{
			const unsigned sweeps = step();
			if (sweeps == 0)
				return 0;

			if (sweeps > worst)
				worst = sweeps;
		}

		return worst;
	}
};

#endif /* clock_h */
