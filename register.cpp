//
//  register.cpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-23.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "register.hpp"
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
		registr_t<N> registr;
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
			registr.in(b); // Test all zeros
			registr.enable(true);
			registr.set(true);
			EXPECT_EQ(registr.out(), b);

			registr.enable(false);
			registr.set(false);
			EXPECT_EQ(registr.out(), b);
			
			b.flip();
			registr.in(b); // Test all ones
			EXPECT_EQ(registr.out(), std::bitset<N>());
			
			registr.set(true);
			registr.set(false);
			registr.enable(true);
			EXPECT_EQ(registr.out(), b);
	
			registr.enable(false);
			EXPECT_EQ(registr.out(), std::bitset<N>());

			for (auto i = 0; i < cnt(); i++)
			{
				b = random_bitset();
				registr.in(b);
				registr.set(true);
				EXPECT_EQ(registr.out(), std::bitset<N>());

				registr.set(false);
				registr.enable(true);
				EXPECT_EQ(registr.out(), b);
				
				registr.enable(false);
				EXPECT_EQ(registr.out(), std::bitset<N>());
			}
		}
	};
}


TEST(basic, register)
{
	MockCallback cb;
	registr_t<> registr;
	registr.attach(cb.cb());
	
	EXPECT_CALL(cb, out(true)).Times(testing::Exactly(1));
	EXPECT_CALL(cb, out(false)).Times(testing::AtLeast(2));
	
	registr.in(false, 0);
	EXPECT_FALSE(registr.out(0));
	
	registr.in(true, 0);
	EXPECT_FALSE(registr.out(0));
	
	registr.set(true);
	EXPECT_FALSE(registr.out(0));

	registr.set(false);
	EXPECT_FALSE(registr.out(0));

	registr.enable(false);
	EXPECT_FALSE(registr.out(0));

	registr.enable(true);
	EXPECT_TRUE(registr.out(0));
}


template<typename T> class fixture_register : public ::testing::Test {};

TYPED_TEST_CASE_P(fixture_register);
TYPED_TEST_P(fixture_register, basic)
{
	TypeParam gate;
	gate.TestBody();
}

REGISTER_TYPED_TEST_CASE_P(fixture_register, basic);
typedef ::testing::Types<tf<2>, tf<4>, tf<8>, tf<16>, tf<32>, tf<64>> PowerOf2Tests;
INSTANTIATE_TYPED_TEST_CASE_P(basic, fixture_register, PowerOf2Tests);
