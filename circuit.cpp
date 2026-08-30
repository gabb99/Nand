//
//  circuit.cpp
//  NAND
//
//  Created by Claude Opus 5 on 2026-08-30.
//  Copyright © 2026 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"

#include "circuit.hpp"

TEST(circuit, nand_truth_table)
{
	for (int a = 0; a < 2; a++)
		for (int b = 0; b < 2; b++)
		{
			circuit_t c;
			circuit_t::node na = c.input(a), nb = c.input(b);
			circuit_t::node o = c.nand_({na, nb});

			ASSERT_NE(c.settle(), 0u) << "did not settle";
			EXPECT_EQ(c.get(o), !(a && b)) << "a=" << a << " b=" << b;
		}
}

TEST(circuit, nand_is_wide)
{
	for (unsigned word = 0; word < 16; word++)
	{
		circuit_t c;
		std::vector<circuit_t::node> ins;
		for (unsigned i = 0; i < 4; i++)
			ins.push_back(c.input((word >> i) & 1));

		circuit_t::node o = c.nand_(ins);

		ASSERT_NE(c.settle(), 0u);
		EXPECT_EQ(c.get(o), word != 0xF) << "word " << word;
	}
}

TEST(circuit, derived_gates)
{
	for (int a = 0; a < 2; a++)
		for (int b = 0; b < 2; b++)
		{
			circuit_t c;
			circuit_t::node na = c.input(a), nb = c.input(b);

			circuit_t::node n_not  = c.not_(na);
			circuit_t::node n_and  = c.and_(na, nb);
			circuit_t::node n_or   = c.or_(na, nb);
			circuit_t::node n_nor  = c.nor_(na, nb);
			circuit_t::node n_xor  = c.xor_(na, nb);
			circuit_t::node n_xnor = c.xnor_(na, nb);

			ASSERT_NE(c.settle(), 0u);

			EXPECT_EQ(c.get(n_not),  !a)         << "a=" << a << " b=" << b;
			EXPECT_EQ(c.get(n_and),  a && b)     << "a=" << a << " b=" << b;
			EXPECT_EQ(c.get(n_or),   a || b)     << "a=" << a << " b=" << b;
			EXPECT_EQ(c.get(n_nor),  !(a || b))  << "a=" << a << " b=" << b;
			EXPECT_EQ(c.get(n_xor),  a != b)     << "a=" << a << " b=" << b;
			EXPECT_EQ(c.get(n_xnor), a == b)     << "a=" << a << " b=" << b;
		}
}

TEST(circuit, oscillation_is_reported_not_fatal)
{
	// An inverter driving itself. wire_t answers this with a stack overflow.
	circuit_t c;
	circuit_t::node n = c.wire();
	c.nand_gate(n, {n, n});

	EXPECT_EQ(c.settle(50), 0u) << "an oscillator must not report as settled";
}

TEST(circuit, sr_latch_holds)
{
	circuit_t c;
	circuit_t::node sbar = c.input(true), rbar = c.input(true);
	auto [q, qbar] = sr_latch(c, sbar, rbar);

	ASSERT_NE(c.settle(), 0u);
	EXPECT_NE(c.get(q), c.get(qbar)) << "outputs must be complementary";

	c.set(sbar, false); ASSERT_NE(c.settle(), 0u);
	c.set(sbar, true);  ASSERT_NE(c.settle(), 0u);
	EXPECT_TRUE(c.get(q)) << "set";

	ASSERT_NE(c.settle(), 0u);
	EXPECT_TRUE(c.get(q)) << "held with both inputs idle";

	c.set(rbar, false); ASSERT_NE(c.settle(), 0u);
	c.set(rbar, true);  ASSERT_NE(c.settle(), 0u);
	EXPECT_FALSE(c.get(q)) << "reset";

	ASSERT_NE(c.settle(), 0u);
	EXPECT_FALSE(c.get(q)) << "held with both inputs idle";
}

TEST(circuit, dlatch_is_transparent_then_holds)
{
	circuit_t c;
	circuit_t::node d = c.input(false), e = c.input(false);
	circuit_t::node q = dlatch(c, d, e);

	ASSERT_NE(c.settle(), 0u);

	c.set(e, true);  ASSERT_NE(c.settle(), 0u);
	c.set(d, true);  ASSERT_NE(c.settle(), 0u);
	EXPECT_TRUE(c.get(q))  << "transparent while enabled";

	c.set(d, false); ASSERT_NE(c.settle(), 0u);
	EXPECT_FALSE(c.get(q)) << "transparent while enabled";

	c.set(d, true);  ASSERT_NE(c.settle(), 0u);
	c.set(e, false); ASSERT_NE(c.settle(), 0u);
	c.set(d, false); ASSERT_NE(c.settle(), 0u);
	EXPECT_TRUE(c.get(q))  << "must hold once disabled - wire_t loses this bit";
}

TEST(circuit, dff_is_edge_triggered)
{
	circuit_t c;
	circuit_t::node d = c.input(false), clk = c.input(false);
	circuit_t::node q = dff(c, d, clk);

	ASSERT_NE(c.settle(), 0u);

	c.set(d, true);    ASSERT_NE(c.settle(), 0u);
	EXPECT_FALSE(c.get(q)) << "nothing happens without an edge";

	c.set(clk, true);  ASSERT_NE(c.settle(), 0u);
	EXPECT_TRUE(c.get(q))  << "captured on the rising edge";

	c.set(d, false);   ASSERT_NE(c.settle(), 0u);
	EXPECT_TRUE(c.get(q))  << "must not be transparent while the clock is high";

	c.set(clk, false); ASSERT_NE(c.settle(), 0u);
	EXPECT_TRUE(c.get(q))  << "still held on the falling edge";

	c.set(clk, true);  ASSERT_NE(c.settle(), 0u);
	EXPECT_FALSE(c.get(q)) << "next edge takes the new value";
}

TEST(circuit, shift_register_does_not_race_through)
{
	// Two flip-flops on ONE clock node. The shared clock fans out to both
	// stages, which is exactly what the wire_t model cannot apply atomically.
	circuit_t c;
	circuit_t::node d = c.input(false), clk = c.input(false);
	circuit_t::node qa = dff(c, d, clk);
	circuit_t::node qb = dff(c, qa, clk);

	ASSERT_NE(c.settle(), 0u);

	auto tick = [&](bool value) {
		c.set(d, value);   ASSERT_NE(c.settle(), 0u);   // D stable before the edge
		c.set(clk, true);  ASSERT_NE(c.settle(), 0u);
		c.set(clk, false); ASSERT_NE(c.settle(), 0u);
	};

	const bool in[] = { true, false, false, true,  true,  false };
	const bool ea[] = { true, false, false, true,  true,  false };
	const bool eb[] = { false, true, false, false, true,  true  };

	for (unsigned i = 0; i < 6; i++)
	{
		tick(in[i]);
		EXPECT_EQ(c.get(qa), ea[i]) << "stage A, tick " << i;
		EXPECT_EQ(c.get(qb), eb[i]) << "stage B, tick " << i;
	}
}

TEST(circuit, settle_reports_gate_delays)
{
	circuit_t c;
	circuit_t::node a = c.input(false), b = c.input(false);
	c.xor_(a, b);

	const unsigned first = c.settle();
	EXPECT_GT(first, 0u);

	// Nothing changed, so the next sweep finds nothing to do.
	EXPECT_EQ(c.settle(), 1u);
}

TEST(circuit, settles_when_input_changes_with_the_edge)
{
	// Changing D in the same sweep as the clock edge is a setup violation.
	// Real hardware would go metastable; the simulator must still converge
	// and give a definite answer rather than spinning. This is the case that
	// rules out a strictly simultaneous update - see settle() in circuit.hpp.
	circuit_t c;
	circuit_t::node d = c.input(false), clk = c.input(false);
	circuit_t::node q = dff(c, d, clk);

	ASSERT_NE(c.settle(), 0u);

	for (unsigned i = 0; i < 4; i++)
	{
		c.set(d, i % 2 == 0);
		c.set(clk, true);
		ASSERT_NE(c.settle(), 0u) << "rising edge, tick " << i;

		c.set(clk, false);
		ASSERT_NE(c.settle(), 0u) << "falling edge, tick " << i;

		EXPECT_EQ(c.get(q), i % 2 != 0) << "captures the pre-edge value, tick " << i;
	}
}
