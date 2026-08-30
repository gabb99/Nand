//
//  clock.cpp
//  NAND
//
//  Created by Claude Opus 5 on 2026-08-30.
//  Copyright © 2026 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"

#include "clock.hpp"

namespace
{
	struct phase_t
	{
		bool clk, clk_delayed, clk_set, clk_enable;
	};

	// phase        clk    clk_d  set    enable
	const phase_t EXPECTED[clk_t::PHASES] = {
		{ false, false, false, false },   // 0  idle
		{ true,  false, false, true  },   // 1  bus driven, nothing capturing
		{ true,  true,  true,  true  },   // 2  capture
		{ false, true,  false, true  },   // 3  capture done, bus still driven
	};
}


TEST(clock, phases)
{
	circuit_t c;
	clk_t clock(c);

	ASSERT_NE(clock.reset(), 0u);

	for (unsigned cycle = 0; cycle < 3; cycle++)
		for (unsigned i = 0; i < clk_t::PHASES; i++)
		{
			const phase_t& e = EXPECTED[clock.phase()];

			EXPECT_EQ(c.get(clock.clk()),         e.clk)         << "phase " << clock.phase();
			EXPECT_EQ(c.get(clock.clk_delayed()), e.clk_delayed) << "phase " << clock.phase();
			EXPECT_EQ(c.get(clock.clk_set()),     e.clk_set)     << "phase " << clock.phase();
			EXPECT_EQ(c.get(clock.clk_enable()),  e.clk_enable)  << "phase " << clock.phase();

			ASSERT_NE(clock.step(), 0u);
		}
}

TEST(clock, set_is_bracketed_by_enable)
{
	circuit_t c;
	clk_t clock(c);

	ASSERT_NE(clock.reset(), 0u);

	unsigned set_high = 0, enable_high = 0;

	for (unsigned i = 0; i < clk_t::PHASES; i++)
	{
		const bool set    = c.get(clock.clk_set());
		const bool enable = c.get(clock.clk_enable());

		// The whole reason the derived signals exist.
		if (set)
			EXPECT_TRUE(enable) << "set pulsed while nothing was driving the bus";

		set_high    += set    ? 1 : 0;
		enable_high += enable ? 1 : 0;

		ASSERT_NE(clock.step(), 0u);
	}

	EXPECT_EQ(set_high,    1u) << "set must be a narrow pulse";
	EXPECT_EQ(enable_high, 3u) << "enable must bracket it on both sides";
}

TEST(clock, wraps_around)
{
	circuit_t c;
	clk_t clock(c);

	ASSERT_NE(clock.reset(), 0u);
	EXPECT_EQ(clock.phase(), 0u);

	ASSERT_NE(clock.tick(), 0u);
	EXPECT_EQ(clock.phase(), 0u) << "a tick is one whole cycle";
}

TEST(clock, costs_five_nands)
{
	// and_ is two gates, or_ is three.
	circuit_t c;
	clk_t clock(c);

	EXPECT_EQ(c.gates(), 5u);
}

TEST(clock, captures_a_latch_only_during_the_set_pulse)
{
	// A latch on clk_set: it must take its data in phase 2 and nowhere else.
	circuit_t c;
	clk_t clock(c);

	circuit_t::node d = c.input(false);
	circuit_t::node q = dlatch(c, d, clock.clk_set());

	ASSERT_NE(clock.reset(), 0u);
	EXPECT_FALSE(c.get(q));

	c.set(d, true);
	ASSERT_NE(c.settle(), 0u);
	EXPECT_FALSE(c.get(q)) << "data alone must not get in";

	ASSERT_NE(clock.step(), 0u);                  // phase 1: enable up, set still low
	EXPECT_FALSE(c.get(q)) << "phase 1 must not capture";

	ASSERT_NE(clock.step(), 0u);                  // phase 2: set pulses
	EXPECT_TRUE(c.get(q))  << "phase 2 must capture";

	ASSERT_NE(clock.step(), 0u);                  // phase 3: set drops
	EXPECT_TRUE(c.get(q))  << "value must survive the falling edge of set";

	c.set(d, false);
	ASSERT_NE(c.settle(), 0u);
	EXPECT_TRUE(c.get(q))  << "held once the pulse is over";

	ASSERT_NE(clock.step(), 0u);                  // phase 0
	EXPECT_TRUE(c.get(q))  << "held across the idle phase";

	// ... and the next set pulse takes the new value.
	ASSERT_NE(clock.step(), 0u);
	ASSERT_NE(clock.step(), 0u);                  // phase 2 again
	EXPECT_FALSE(c.get(q)) << "next pulse captures the new data";
}

TEST(clock, drives_a_flip_flop)
{
	circuit_t c;
	clk_t clock(c);

	circuit_t::node d = c.input(false);
	circuit_t::node q = dff(c, d, clock.clk());

	ASSERT_NE(clock.reset(), 0u);
	EXPECT_FALSE(c.get(q));

	const bool sequence[] = { true, true, false, true, false, false };

	for (unsigned i = 0; i < 6; i++)
	{
		c.set(d, sequence[i]);
		ASSERT_NE(c.settle(), 0u);          // data stable before the edge

		ASSERT_NE(clock.tick(), 0u);
		EXPECT_EQ(c.get(q), sequence[i]) << "cycle " << i;
	}
}
