//
//  xor.cpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-22.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "xor.hpp"

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
		MockCallback cb;
		xor_t<N> xor_;
		std::bitset<N> b;
		
	public:
		void TestBody()
		{
			xor_.attach(cb.cb());
			
			EXPECT_CALL(cb, out(false)).Times(testing::AtLeast(N));
			EXPECT_CALL(cb, out(true)).Times(testing::Exactly(N));
			
			xor_.in(b); // Test all zeros
			EXPECT_FALSE(xor_.out());
			
			for (auto i = 0; i < N; i++)
			{
				xor_.in(true, i);
				EXPECT_TRUE(xor_.out());
				xor_.in(false, i);
				EXPECT_FALSE(xor_.out());
			}
			
			b.flip();
			xor_.in(b); // Test all ones
			EXPECT_FALSE(xor_.out());
		}
	};
}


TEST(basic, xor)
{
	xor_t<> xor_;
	
	xor_.in(false, 0);
	xor_.in(false, 1);
	EXPECT_FALSE(xor_.out());
	
	xor_.in({true, false});
	EXPECT_TRUE(xor_.out());
	
	xor_.in(false, 0);
	xor_.in(true, 1);
	EXPECT_TRUE(xor_.out());
	
	xor_.in({true, true});
	EXPECT_FALSE(xor_.out());
}

template<typename T> class fixture_xor : public ::testing::Test {};

TYPED_TEST_CASE_P(fixture_xor);
TYPED_TEST_P(fixture_xor, basic)
{
	TypeParam gate;
	gate.TestBody();
}

REGISTER_TYPED_TEST_CASE_P(fixture_xor, basic);
typedef ::testing::Types<tf<2>, tf<4>, tf<8>, tf<16>, tf<32>, tf<64>> PowerOf2Tests;
INSTANTIATE_TYPED_TEST_CASE_P(basic, fixture_xor, PowerOf2Tests);
