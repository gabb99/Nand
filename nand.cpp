//
//  nand.cpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-22.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "nand.hpp"

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
		nand_t<N> nand;
		std::bitset<N> b;
		
	public:
		void TestBody()
		{
			nand.attach(cb.cb());
			
			EXPECT_CALL(cb, out(true)).Times(testing::AtLeast(N));
			EXPECT_CALL(cb, out(false)).Times(testing::Exactly(1));
			
			nand.in(b); // Test all zeros
			EXPECT_TRUE(nand.out());
			
			for (auto i = 0; i < N; i++)
			{
				nand.in(true, i);
				EXPECT_TRUE(nand.out());
				nand.in(false, i);
				EXPECT_TRUE(nand.out());
			}
			
			b.flip();
			nand.in(b); // Test all ones
			EXPECT_FALSE(nand.out());
		}
	};
}


TEST(basic, nand)
{
	MockCallback cb;
	nand_t<> nand;
	nand.attach(cb.cb());
	
	EXPECT_CALL(cb, out(true)).Times(testing::AtLeast(3));
	EXPECT_CALL(cb, out(false)).Times(testing::Exactly(1));
	
	nand.in(false, 0);
	nand.in(false, 1);
	EXPECT_TRUE(nand.out());
	
	nand.in({true, false});
	EXPECT_TRUE(nand.out());
	
	nand.in(std::bitset<2>("10"));
	EXPECT_TRUE(nand.out());
	
	nand.in({true, true});
	EXPECT_FALSE(nand.out());
}


template<typename T> class fixture_nand : public ::testing::Test {};

TYPED_TEST_CASE_P(fixture_nand);
TYPED_TEST_P(fixture_nand, basic)
{
	TypeParam gate;
	gate.TestBody();
}

REGISTER_TYPED_TEST_CASE_P(fixture_nand, basic);
typedef ::testing::Types<tf<2>, tf<4>, tf<8>, tf<16>, tf<32>, tf<64>> PowerOf2Tests;
INSTANTIATE_TYPED_TEST_CASE_P(basic, fixture_nand, PowerOf2Tests);
