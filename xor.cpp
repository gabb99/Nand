//
//  xor.cpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-22.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"

#include "probe.hpp"

#include "xor.hpp"

namespace
{
	
	template <unsigned N>
	class tf
	{
		probe_t p;
		xor_t<N> xor_;
		std::bitset<N> b;
		
	public:
		void TestBody()
		{
			xor_.attach(p.cb());
			p.seed(xor_.out());
			
			
			xor_.in(b); // Test all zeros
			EXPECT_FALSE(xor_.out());
			EXPECT_TRUE(delivered(p, xor_.out()));
			
			for (auto i = 0; i < N; i++)
			{
				xor_.in(true, i);
				EXPECT_TRUE(xor_.out());
				EXPECT_TRUE(delivered(p, xor_.out()));
				xor_.in(false, i);
				EXPECT_FALSE(xor_.out());
				EXPECT_TRUE(delivered(p, xor_.out()));
			}
			
			b.flip();
			xor_.in(b); // Test all ones
			EXPECT_FALSE(xor_.out());
			EXPECT_TRUE(delivered(p, xor_.out()));
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

TYPED_TEST_SUITE_P(fixture_xor);
TYPED_TEST_P(fixture_xor, basic)
{
	TypeParam gate;
	gate.TestBody();
}

REGISTER_TYPED_TEST_SUITE_P(fixture_xor, basic);
typedef ::testing::Types<tf<2>, tf<4>, tf<8>, tf<16>, tf<32>, tf<64>> PowerOf2Tests;
INSTANTIATE_TYPED_TEST_SUITE_P(basic, fixture_xor, PowerOf2Tests);
