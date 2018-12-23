//
//  xnor.cpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-22.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "xnor.hpp"

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
		xnor_t<N> xnor_;
		std::bitset<N> b;
		
	public:
		void TestBody()
		{
			xnor_.attach(cb.cb());
			
			EXPECT_CALL(cb, out(false)).Times(testing::AtLeast(N));
			EXPECT_CALL(cb, out(true)).Times(testing::Exactly(N + 2));
			
			xnor_.in(b); // Test all zeros
			EXPECT_TRUE(xnor_.out());
			
			for (auto i = 0; i < N; i++)
			{
				xnor_.in(true, i);
				EXPECT_FALSE(xnor_.out());
				xnor_.in(false, i);
				EXPECT_TRUE(xnor_.out());
			}
			
			b.flip();
			xnor_.in(b); // Test all ones
			EXPECT_TRUE(xnor_.out());
		}
	};
}


TEST(basic, xnor)
{
	xnor_t<> xnor_;
	
	xnor_.in(false, 0);
	xnor_.in(false, 1);
	EXPECT_TRUE(xnor_.out());
	
	xnor_.in({true, false});
	EXPECT_FALSE(xnor_.out());
	
	xnor_.in(false, 0);
	xnor_.in(true, 1);
	EXPECT_FALSE(xnor_.out());
	
	xnor_.in({true, true});
	EXPECT_TRUE(xnor_.out());
}

template<typename T> class fixture_xnor : public ::testing::Test {};

TYPED_TEST_CASE_P(fixture_xnor);
TYPED_TEST_P(fixture_xnor, basic)
{
	TypeParam gate;
	gate.TestBody();
}

REGISTER_TYPED_TEST_CASE_P(fixture_xnor, basic);
typedef ::testing::Types<tf<2>, tf<4>, tf<8>, tf<16>, tf<32>, tf<64>> PowerOf2Tests;
INSTANTIATE_TYPED_TEST_CASE_P(basic, fixture_xnor, PowerOf2Tests);
