//
//  nor.cpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-22.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "nor.hpp"

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
		nor_t<N> nor_;
		std::bitset<N> b;
		
	public:
		void TestBody()
		{
			nor_.attach(cb.cb());
			
			EXPECT_CALL(cb, out(false)).Times(testing::AtLeast(N));
			EXPECT_CALL(cb, out(true)).Times(testing::Exactly(N + 1));
			
			nor_.in(b); // Test all zeros
			EXPECT_TRUE(nor_.out());
			
			for (auto i = 0; i < N; i++)
			{
				nor_.in(true, i);
				EXPECT_FALSE(nor_.out());
				nor_.in(false, i);
				EXPECT_TRUE(nor_.out());
			}
			
			b.flip();
			nor_.in(b); // Test all ones
			EXPECT_FALSE(nor_.out());
		}
	};
}


TEST(basic, nor)
{
	nor_t<> nor_;
	
	nor_.in(false, 0);
	nor_.in(false, 1);
	EXPECT_TRUE(nor_.out());
	
	nor_.in({true, false});
	EXPECT_FALSE(nor_.out());
	
	nor_.in(false, 0);
	nor_.in(true, 1);
	EXPECT_FALSE(nor_.out());
	
	nor_.in({true, true});
	EXPECT_FALSE(nor_.out());
}

template<typename T> class fixture_nor : public ::testing::Test {};

TYPED_TEST_CASE_P(fixture_nor);
TYPED_TEST_P(fixture_nor, basic)
{
	TypeParam gate;
	gate.TestBody();
}

REGISTER_TYPED_TEST_CASE_P(fixture_nor, basic);
typedef ::testing::Types<tf<2>, tf<4>, tf<8>, tf<16>, tf<32>, tf<64>> PowerOf2Tests;
INSTANTIATE_TYPED_TEST_CASE_P(basic, fixture_nor, PowerOf2Tests);
