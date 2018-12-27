//
//  enabler.cpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-23.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "enabler.hpp"
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
		enabler_t<N> enabler;
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
			enabler.in(b); // Test all zeros
			enabler.enable(true);
			EXPECT_EQ(enabler.out(), b);
			
			enabler.enable(false);
			EXPECT_EQ(enabler.out(), b);

			b.flip();
			enabler.in(b); // Test all ones
			EXPECT_EQ(enabler.out(), std::bitset<N>());
		
			enabler.enable(true);
			EXPECT_EQ(enabler.out(), b);

			for (auto i = 0; i < cnt(); i++)
			{
				b = random_bitset();
				enabler.in(b);
				enabler.enable(true);
				EXPECT_EQ(enabler.out(), b);

				enabler.enable(false);
				EXPECT_EQ(enabler.out(), std::bitset<N>());
			}
		}
	};
}


TEST(basic, enabler)
{
	MockCallback cb;
	enabler_t<> enabler;
	enabler.attach(cb.cb());
	
	EXPECT_CALL(cb, out(true)).Times(testing::Exactly(1));
	EXPECT_CALL(cb, out(false)).Times(testing::Exactly(2));

	enabler.in(false, 0);
	EXPECT_FALSE(enabler.out(0));

	enabler.in(true, 0);
	EXPECT_FALSE(enabler.out(0));

	enabler.enable(true);
	EXPECT_TRUE(enabler.out(0));
}


template<typename T> class fixture_enabler : public ::testing::Test {};

TYPED_TEST_CASE_P(fixture_enabler);
TYPED_TEST_P(fixture_enabler, basic)
{
	TypeParam gate;
	gate.TestBody();
}

REGISTER_TYPED_TEST_CASE_P(fixture_enabler, basic);
typedef ::testing::Types<tf<2>, tf<4>, tf<8>, tf<16>, tf<32>, tf<64>> PowerOf2Tests;
INSTANTIATE_TYPED_TEST_CASE_P(basic, fixture_enabler, PowerOf2Tests);
