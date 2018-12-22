//
//  and.cpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-20.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "and.hpp"

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
		and_t<N> and_;
		std::bitset<N> b;
		
	public:
		void TestBody()
		{
			and_.attach(cb.cb());
			
			EXPECT_CALL(cb, out(true)).Times(testing::AtLeast(N));
			EXPECT_CALL(cb, out(false)).Times(testing::Exactly(1));
			
			and_.in(b); // Test all zeros
			EXPECT_FALSE(and_.out());
			
			for (auto i = 0; i < N; i++)
			{
				and_.in(true, i);
				EXPECT_FALSE(and_.out());
				and_.in(false, i);
			}
			
			b.flip();
			and_.in(b); // Test all ones
			EXPECT_TRUE(and_.out());
		}
	};
}


TEST(basic, and)
{
	and_t<> and_;
	
	and_.in(false, 0);
	and_.in(false, 1);
	EXPECT_FALSE(and_.out());
	
	and_.in({true, false});
	EXPECT_FALSE(and_.out());
	
	and_.in(false, 0);
	and_.in(true, 1);
	EXPECT_FALSE(and_.out());
	
	and_.in({true, true});
	EXPECT_TRUE(and_.out());
}

template<typename T> class fixture_and : public ::testing::Test {};

TYPED_TEST_CASE_P(fixture_and);
TYPED_TEST_P(fixture_and, basic)
{
	TypeParam gate;
	gate.TestBody();
}

REGISTER_TYPED_TEST_CASE_P(fixture_and, basic);
typedef ::testing::Types<tf<2>, tf<4>, tf<8>, tf<16>, tf<32>, tf<64>> PowerOf2Tests;
INSTANTIATE_TYPED_TEST_CASE_P(basic, fixture_and, PowerOf2Tests);
