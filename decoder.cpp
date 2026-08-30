//
//  decoder.cpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-23.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"

#include "probe.hpp"

#include "decoder.hpp"

namespace
{
	// Feeds every possible word to a decoder_t<N> and checks that exactly the
	// matching line is high, both by polling out() and by what the output
	// wires last delivered.
	template <unsigned N>
	void exhaustive()
	{
		constexpr unsigned SIZE = decoder_t<N>::SIZE;

		decoder_t<N> decoder;
		std::array<probe_t, SIZE> p;

		for (unsigned i = 0; i < SIZE; i++)
		{
			decoder.attach(p[i].cb(), i);
			p[i].seed(decoder.out(i));
		}

		for (unsigned word = 0; word < SIZE; word++)
		{
			decoder.in(std::bitset<N>(word));

			std::bitset<SIZE> expected;
			expected.set(word);

			EXPECT_EQ(decoder.out(), expected) << "word " << word;

			for (unsigned i = 0; i < SIZE; i++)
				EXPECT_TRUE(delivered(p[i], expected.test(i)))
					<< "line " << i << ", word " << word;
		}
	}
}


TEST(basic, decoder_1) { exhaustive<1>(); }
TEST(basic, decoder_2) { exhaustive<2>(); }
TEST(basic, decoder_3) { exhaustive<3>(); }
TEST(basic, decoder_4) { exhaustive<4>(); }


TEST(basic, decoder_zero)
{
	// All zeros selects line 0, and the constructor settles the gates so that
	// this holds before anything is fed in.
	decoder_t<3> decoder;

	std::bitset<decoder_t<3>::SIZE> expected;
	expected.set(0);

	EXPECT_EQ(decoder.out(), expected);
}


TEST(basic, decoder_callback)
{
	decoder_t<2> decoder;
	probe_t p;

	decoder.attach(p.cb(), 1);
	p.seed(decoder.out(1));

	decoder.in(std::bitset<2>(1));
	EXPECT_TRUE(decoder.out(1));
	EXPECT_TRUE(delivered(p, decoder.out(1)));

	decoder.in(std::bitset<2>(2));
	EXPECT_FALSE(decoder.out(1));
	EXPECT_TRUE(delivered(p, decoder.out(1)));
}
