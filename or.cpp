//
//  or.cpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-20.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"

#include "probe.hpp"

#include "or.hpp"

namespace
{
	
	template <unsigned N>
	class tf
	{
		probe_t p;
		or_t<N> or_;
		std::bitset<N> b;
		
	public:
		void TestBody()
		{
			or_.attach(p.cb());
			p.seed(or_.out());
			
			
			or_.in(b); // Test all zeros
			EXPECT_FALSE(or_.out());
			EXPECT_TRUE(delivered(p, or_.out()));
			
			for (auto i = 0; i < N; i++)
			{
				or_.in(true, i);
				EXPECT_TRUE(or_.out());
				EXPECT_TRUE(delivered(p, or_.out()));
				or_.in(false, i);
				EXPECT_FALSE(or_.out());
				EXPECT_TRUE(delivered(p, or_.out()));
			}
			
			b.flip();
			or_.in(b); // Test all ones
			EXPECT_TRUE(or_.out());
			EXPECT_TRUE(delivered(p, or_.out()));
		}
	};
}

TEST(basic, or)
{
	or_t<> or_;
	
	or_.in(false, 0);
	or_.in(false, 1);
	EXPECT_FALSE(or_.out());
	
	or_.in({true, false});
	EXPECT_TRUE(or_.out());
	
	or_.in(false, 0);
	or_.in(true, 1);
	EXPECT_TRUE(or_.out());
	
	or_.in({true, true});
	EXPECT_TRUE(or_.out());
}

template<typename T> class fixture_or : public ::testing::Test {};

TYPED_TEST_SUITE_P(fixture_or);
TYPED_TEST_P(fixture_or, basic)
{
	TypeParam gate;
	gate.TestBody();
}

REGISTER_TYPED_TEST_SUITE_P(fixture_or, basic);
typedef ::testing::Types<tf<2>, tf<4>, tf<8>, tf<16>, tf<32>, tf<64>> PowerOf2Tests;
INSTANTIATE_TYPED_TEST_SUITE_P(basic, fixture_or, PowerOf2Tests);
