//
//  wire.cpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-20.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//
//  Moved out of main.cpp on 2026-08-30.
//

#include "gtest/gtest.h"

#include "probe.hpp"

#include "wire.hpp"

TEST(basic, wire)
{
	probe_t p;
	wire_t<> wire;
	wire.attach(p.cb());

	// A wire is pure fan-out: every in() is delivered once to each callback
	// attached to that line. Here the count IS the contract, not a by-product
	// of how far a change propagated, so counting it is fair game.
	wire.in(true);
	EXPECT_EQ(p.calls(), 1u);
	EXPECT_TRUE(delivered(p, true));

	wire.in(false);
	EXPECT_EQ(p.calls(), 2u);
	EXPECT_TRUE(delivered(p, false));
}

TEST(basic, wire_n)
{
	probe_t p;
	wire_t<4> wire;

	for (unsigned i = 0; i < wire.size(); i++)
		wire.attach(p.cb(), i);

	wire.in(true, 0);
	wire.in(true, 2);
	EXPECT_EQ(p.calls(), 2u);
	EXPECT_TRUE(delivered(p, true));

	wire.in(false, 1);
	wire.in(false, 3);
	EXPECT_EQ(p.calls(), 4u);
	EXPECT_TRUE(delivered(p, false));
}

// Each line of a wire_t<N> is independent: driving one must not disturb another.
TEST(basic, wire_lines_are_independent)
{
	std::array<probe_t, 4> p;
	wire_t<4> wire;

	for (unsigned i = 0; i < wire.size(); i++)
		wire.attach(p[i].cb(), i);

	wire.in(true, 2);

	EXPECT_FALSE(p[0].driven());
	EXPECT_FALSE(p[1].driven());
	EXPECT_TRUE(delivered(p[2], true));
	EXPECT_FALSE(p[3].driven());
}
