//
//  dgate.cpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-24.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"

#include "probe.hpp"

#include "dgate.hpp"
#include <random>

namespace
{
	
	template <unsigned N>
	class tf
	{
		dgate_t<N> dgate;
		std::bitset<N> b;
		
		constexpr unsigned cnt()
		{
			if (N >= 16)
				return 256;

			return 1 << N;
		}
		
		typename std::bitset<N> random_bitset( double p = 0.5)
		{
			typename std::bitset<N> bits;
			std::random_device rd;
			std::mt19937 gen(rd());
			std::bernoulli_distribution d(p);
			
			for( int n = 0; n < N; ++n)
			{
				bits[n] = d(gen);
			}
			
			return bits;
		}

	public:
		void TestBody()
		{
			dgate.in(b); // Test all zeros
			EXPECT_EQ(dgate.out(), b);
			
			b.flip();
			dgate.in(b); // Test all ones
			EXPECT_NE(dgate.out(), b);

			dgate.set(true);
			EXPECT_EQ(dgate.out(), b);

			dgate.set(false);
			b.flip();
			dgate.in(b); // Test all zeros
			EXPECT_NE(dgate.out(), b);

			// Restore to the set values
			b.flip();

			for (auto i = 0; i < cnt(); i++)
			{
				// Skip over same bit sequences
				auto r = random_bitset();
				if (r == b)
					continue;

				dgate.in(b = r);
				EXPECT_NE(dgate.out(), b);

				dgate.set(true);
				EXPECT_EQ(dgate.out(), b);

				dgate.set(false);
				dgate.in(std::bitset<N>());

				EXPECT_EQ(dgate.out(), b);
			}
		}
	};
}


TEST(basic, dgate)
{
	probe_t p;
	dgate_t<> dgate;
	dgate.attach(p.cb());
	p.seed(dgate.out(0));


	// pass through, when not set
	{
		dgate.in(false, 0);
		EXPECT_FALSE(dgate.out(0));
		EXPECT_TRUE(delivered(p, dgate.out(0)));
		
		dgate.in(true, 0);
		EXPECT_FALSE(dgate.out(0));
		EXPECT_TRUE(delivered(p, dgate.out(0)));
	}

	// Remember true
	{
		dgate.set(true);
		EXPECT_TRUE(dgate.out(0));
		EXPECT_TRUE(delivered(p, dgate.out(0)));

		dgate.set(false);
		EXPECT_TRUE(dgate.out(0));
		EXPECT_TRUE(delivered(p, dgate.out(0)));

		dgate.in(true, 0);
		EXPECT_TRUE(dgate.out(0));
		EXPECT_TRUE(delivered(p, dgate.out(0)));

		dgate.in(false, 0);
		EXPECT_TRUE(dgate.out(0));
		EXPECT_TRUE(delivered(p, dgate.out(0)));
	}

	// Remember false
	{
		dgate.set(true);
		EXPECT_FALSE(dgate.out(0));
		EXPECT_TRUE(delivered(p, dgate.out(0)));
		
		dgate.set(false);
		EXPECT_FALSE(dgate.out(0));
		EXPECT_TRUE(delivered(p, dgate.out(0)));
		
		dgate.in(true, 0);
		
		(dgate.out(0));
		
		dgate.in(false, 0);
		EXPECT_FALSE(dgate.out(0));
		EXPECT_TRUE(delivered(p, dgate.out(0)));
	}
}


template<typename T> class fixture_dgate : public ::testing::Test {};

TYPED_TEST_SUITE_P(fixture_dgate);
TYPED_TEST_P(fixture_dgate, basic)
{
	TypeParam gate;
	gate.TestBody();
}

REGISTER_TYPED_TEST_SUITE_P(fixture_dgate, basic);
typedef ::testing::Types<tf<2>, tf<4>, tf<8>, tf<16>, tf<32>, tf<64>> PowerOf2Tests;
INSTANTIATE_TYPED_TEST_SUITE_P(basic, fixture_dgate, PowerOf2Tests);


TEST(basic, dgate_is_built_from_nands)
{
	// Four NANDs per bit: the two input gates and the cross-coupled pair.
	EXPECT_EQ(dgate_t<1>().gates(),  4u);
	EXPECT_EQ(dgate_t<8>().gates(),  32u);
	EXPECT_EQ(dgate_t<64>().gates(), 256u);
}

TEST(basic, dgate_bits_are_independent)
{
	// One enable line fans out to all 8 latches. Applying it is exactly the
	// write the old model could not do atomically.
	dgate_t<8> dgate;

	const std::bitset<8> pattern("10110010");

	dgate.in(pattern);
	dgate.set(true);
	EXPECT_EQ(dgate.out(), pattern) << "transparent while set";

	dgate.set(false);
	EXPECT_EQ(dgate.out(), pattern) << "held when set drops";

	// Hammer every data line while held; not one bit may move.
	for (unsigned i = 0; i < 8; i++)
	{
		dgate.in(true, i);
		EXPECT_EQ(dgate.out(), pattern) << "bit " << i << " driven high while held";

		dgate.in(false, i);
		EXPECT_EQ(dgate.out(), pattern) << "bit " << i << " driven low while held";
	}

	dgate.in(std::bitset<8>());
	EXPECT_EQ(dgate.out(), pattern) << "whole word cleared while held";
}

TEST(basic, dgate_follows_d_while_transparent)
{
	dgate_t<4> dgate;
	dgate.set(true);

	for (unsigned word = 0; word < 16; word++)
	{
		dgate.in(std::bitset<4>(word));
		EXPECT_EQ(dgate.out(), std::bitset<4>(word)) << "word " << word;
	}

	dgate.set(false);
	const std::bitset<4> held = dgate.out();

	for (unsigned word = 0; word < 16; word++)
	{
		dgate.in(std::bitset<4>(word));
		EXPECT_EQ(dgate.out(), held) << "word " << word << " must not get through";
	}
}
