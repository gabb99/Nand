//
//  nand.cpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-22.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"

#include "probe.hpp"

#include "nand.hpp"

namespace
{
	
	template <unsigned N>
	class tf
	{
		probe_t p;
		nand_t<N> nand;
		std::bitset<N> b;
		
	public:
		void TestBody()
		{
			nand.attach(p.cb());
			p.seed(nand.out());
			
			
			nand.in(b); // Test all zeros
			EXPECT_TRUE(nand.out());
			EXPECT_TRUE(delivered(p, nand.out()));
			
			for (auto i = 0; i < N; i++)
			{
				nand.in(true, i);
				EXPECT_TRUE(nand.out());
				EXPECT_TRUE(delivered(p, nand.out()));
				nand.in(false, i);
				EXPECT_TRUE(nand.out());
				EXPECT_TRUE(delivered(p, nand.out()));
			}
			
			b.flip();
			nand.in(b); // Test all ones
			EXPECT_FALSE(nand.out());
			EXPECT_TRUE(delivered(p, nand.out()));
		}
	};
}


TEST(basic, nand)
{
	probe_t p;
	nand_t<> nand;
	nand.attach(p.cb());
	p.seed(nand.out());
	
	
	nand.in(false, 0);
	nand.in(false, 1);
	EXPECT_TRUE(nand.out());
	EXPECT_TRUE(delivered(p, nand.out()));
	
	nand.in({true, false});
	EXPECT_TRUE(nand.out());
	EXPECT_TRUE(delivered(p, nand.out()));
	
	nand.in(std::bitset<2>("10"));
	EXPECT_TRUE(nand.out());
	EXPECT_TRUE(delivered(p, nand.out()));
	
	nand.in({true, true});
	EXPECT_FALSE(nand.out());
	EXPECT_TRUE(delivered(p, nand.out()));
}


template<typename T> class fixture_nand : public ::testing::Test {};

TYPED_TEST_SUITE_P(fixture_nand);
TYPED_TEST_P(fixture_nand, basic)
{
	TypeParam gate;
	gate.TestBody();
}

REGISTER_TYPED_TEST_SUITE_P(fixture_nand, basic);
typedef ::testing::Types<tf<2>, tf<4>, tf<8>, tf<16>, tf<32>, tf<64>> PowerOf2Tests;
INSTANTIATE_TYPED_TEST_SUITE_P(basic, fixture_nand, PowerOf2Tests);
