//
//  dgate.cpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-24.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "dgate.hpp"
#include <random>

namespace
{
	class Callback
	{
	public:
		Callback() {}
		virtual ~Callback() {}
		
		std::function<void(bool)> cb() { return [&](bool value) { out(value); }; }
		virtual void out(bool) {}
	};
	
	class MockCallback : public Callback
	{
	public:
		MOCK_METHOD1(out, void(bool));
	};
	
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
	MockCallback cb;
	dgate_t<> dgate;
	dgate.attach(cb.cb());

	EXPECT_CALL(cb, out(true)).Times(testing::AtLeast(2));
	EXPECT_CALL(cb, out(false)).Times(testing::AtLeast(4));

	// pass through, when not set
	{
		dgate.in(false, 0);
		EXPECT_FALSE(dgate.out(0));
		
		dgate.in(true, 0);
		EXPECT_FALSE(dgate.out(0));
	}

	// Remember true
	{
		dgate.set(true);
		EXPECT_TRUE(dgate.out(0));

		dgate.set(false);
		EXPECT_TRUE(dgate.out(0));

		dgate.in(true, 0);
		EXPECT_TRUE(dgate.out(0));

		dgate.in(false, 0);
		EXPECT_TRUE(dgate.out(0));
	}

	// Remember false
	{
		dgate.set(true);
		EXPECT_FALSE(dgate.out(0));
		
		dgate.set(false);
		EXPECT_FALSE(dgate.out(0));
		
		dgate.in(true, 0);
		
		(dgate.out(0));
		
		dgate.in(false, 0);
		EXPECT_FALSE(dgate.out(0));
	}
}


template<typename T> class fixture_dgate : public ::testing::Test {};

TYPED_TEST_CASE_P(fixture_dgate);
TYPED_TEST_P(fixture_dgate, basic)
{
	TypeParam gate;
	gate.TestBody();
}

REGISTER_TYPED_TEST_CASE_P(fixture_dgate, basic);
typedef ::testing::Types<tf<2>, tf<4>, tf<8>, tf<16>, tf<32>, tf<64>> PowerOf2Tests;
INSTANTIATE_TYPED_TEST_CASE_P(basic, fixture_dgate, PowerOf2Tests);
