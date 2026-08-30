//
//  nor.cpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-22.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"

#include "probe.hpp"

#include "nor.hpp"

namespace
{
	
	template <unsigned N>
	class tf
	{
		probe_t p;
		nor_t<N> nor_;
		std::bitset<N> b;
		
	public:
		void TestBody()
		{
			nor_.attach(p.cb());
			p.seed(nor_.out());
			
			
			nor_.in(b); // Test all zeros
			EXPECT_TRUE(nor_.out());
			EXPECT_TRUE(delivered(p, nor_.out()));
			
			for (auto i = 0; i < N; i++)
			{
				nor_.in(true, i);
				EXPECT_FALSE(nor_.out());
				EXPECT_TRUE(delivered(p, nor_.out()));
				nor_.in(false, i);
				EXPECT_TRUE(nor_.out());
				EXPECT_TRUE(delivered(p, nor_.out()));
			}
			
			b.flip();
			nor_.in(b); // Test all ones
			EXPECT_FALSE(nor_.out());
			EXPECT_TRUE(delivered(p, nor_.out()));
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

TYPED_TEST_SUITE_P(fixture_nor);
TYPED_TEST_P(fixture_nor, basic)
{
	TypeParam gate;
	gate.TestBody();
}

REGISTER_TYPED_TEST_SUITE_P(fixture_nor, basic);
typedef ::testing::Types<tf<2>, tf<4>, tf<8>, tf<16>, tf<32>, tf<64>> PowerOf2Tests;
INSTANTIATE_TYPED_TEST_SUITE_P(basic, fixture_nor, PowerOf2Tests);
